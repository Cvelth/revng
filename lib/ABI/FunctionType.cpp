/// \file FunctionType.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_set>

#include "revng/ABI/FunctionType.h"
#include "revng/ABI/ModelAlignment.h"
#include "revng/ABI/Predefined.h"
#include "revng/ABI/RegisterOrder.h"
#include "revng/ABI/RegisterStateDeductions.h"
#include "revng/ADT/STLExtras.h"
#include "revng/ADT/SmallMap.h"
#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/VerifyHelper.h"
#include "revng/Support/EnumSwitch.h"

namespace abi::FunctionType {

using RegisterVector = std::vector<model::Register::Values>;

template<model::Architecture::Values Architecture, typename RegisterType>
bool verify(const SortedVector<RegisterType> &UsedRegisters,
            const RegisterVector &AllowedRegisters) {
  for (const model::Register::Values &Register : AllowedRegisters) {
    // Verify the architecture of allowed registers.
    if (model::Register::getArchitecture(Register) != Architecture)
      revng_abort();

    // Verify that there are no duplicate allowed registers.
    if (llvm::count(AllowedRegisters, Register) != 1)
      revng_abort();
  }

  for (const RegisterType &Register : UsedRegisters) {
    // Verify the architecture of used registers.
    if (model::Register::getArchitecture(Register.Location) != Architecture)
      revng_abort();
  }

  // Verify that every used register is also allowed.
  for (const RegisterType &Register : UsedRegisters)
    if (llvm::count(AllowedRegisters, Register.Location) != 1)
      return false;

  return true;
}

/// A helper used to differentiate vector registers.
///
/// \param Register Any CPU register the model is aware of.
///
/// \return `true` if `Register` is a vector register, `false` otherwise.
constexpr bool isVectorRegisterHelper(model::Register::Values Register) {
  namespace PrimitiveTypeKind = model::PrimitiveTypeKind;
  return model::Register::primitiveKind(Register) == PrimitiveTypeKind::Float;
}

/// Helps detecting unsupported ABI trait definition with respect to
/// the way they return the return values.
///
/// This is an important piece of abi trait verification. For more information
/// see the `static_assert` that invokes it in \ref distributeArguments
///
/// \note Make this function `consteval` after clang-14 is available.
///
/// \return `true` if the ABI is valid, `false` otherwise.
static bool verifyReturnValueLocationHelper(const abi::Definition &D) {
  // Skip ABIs that do not allow returning big values.
  // They do not benefit from this check.
  if (D.ReturnValueLocationRegister != model::Register::Invalid) {
    if (isVectorRegisterHelper(D.ReturnValueLocationRegister)) {
      // Vector register used as the return value locations are not supported.
      return false;
    } else if (llvm::is_contained(D.CalleeSavedRegisters,
                                  D.ReturnValueLocationRegister)) {
      // Using callee saved register as a return value location doesn't make
      // much sense: filter those out.
      return false;
    } else {
      // The return value location register can optionally also be the first
      // GPRs, but only the first one.
      const auto &GPRs = D.GeneralPurposeArgumentRegisters;
      const auto Iterator = llvm::find(GPRs, D.ReturnValueLocationRegister);
      if (Iterator != GPRs.end() && Iterator != GPRs.begin())
        return false;
    }
  }

  return true;
}

static void replaceReferences(const model::Type::Key &OldKey,
                              const model::TypePath &NewTypePath,
                              TupleTree<model::Binary> &Model) {
  auto Visitor = [&](model::TypePath &Visited) {
    if (!Visited.isValid())
      return; // Ignore empty references

    model::Type *Current = Visited.get();
    revng_assert(Current != nullptr);

    if (Current->key() == OldKey)
      Visited = NewTypePath;
  };
  Model.visitReferences(Visitor);
  Model->Types.erase(OldKey);
}

namespace ModelArch = model::Architecture;

template<model::ABI::Values ABI>
class ConversionHelper {
  static constexpr auto Architecture = model::ABI::getArchitecture(ABI);
  static constexpr auto RegisterSize = ModelArch::getPointerSize(Architecture);

  using IndexType = decltype(model::Argument::Index);
  using RegisterList = llvm::SmallVector<model::Register::Values, 1>;
  struct DistributedArgument {
    RegisterList Registers = {};
    size_t Size = 0, SizeOnStack = 0;
  };
  using DistributedArguments = llvm::SmallVector<DistributedArgument, 4>;

  using ArgumentVector = SortedVector<model::Argument>;
  using TypeVector = llvm::SmallVector<UpcastablePointer<model::Type>, 16>;

public:
  static std::optional<model::TypePath>
  toCABI(const model::RawFunctionType &Function,
         TupleTree<model::Binary> &TheBinary) {
    const abi::Definition &D = abi::predefined::get(ABI);

    static constexpr auto Arch = model::ABI::getArchitecture(ABI);
    if (!verify<Arch>(Function.Arguments, D.GeneralPurposeArgumentRegisters))
      return std::nullopt;
    if (!verify<Arch>(Function.ReturnValues,
                      D.GeneralPurposeReturnValueRegisters))
      return std::nullopt;

    // Verify the architecture of return value location register if present.
    const model::Register::Values PTCRR = D.ReturnValueLocationRegister;
    if (PTCRR != model::Register::Invalid)
      revng_assert(model::Register::getArchitecture(PTCRR) == Arch);

    // Verify the architecture of callee saved registers.
    for (auto &SavedRegister : D.CalleeSavedRegisters)
      revng_assert(model::Register::getArchitecture(SavedRegister) == Arch);

    model::CABIFunctionType Result;
    Result.ID = Function.ID;
    Result.CustomName = Function.CustomName;
    Result.OriginalName = Function.OriginalName;
    Result.ABI = ABI;

    // A single place to conveniently store all the types that need to be added
    // to the model in order for the new function prototype to be valid.
    TypeVector TypeDependencies;

    // Convert all the arguments.
    const auto &GPAR = D.GeneralPurposeArgumentRegisters;
    auto RegisterArguments = tryConvertingRegisterArguments(Function.Arguments,
                                                            GPAR,
                                                            *TheBinary,
                                                            TypeDependencies);
    if (RegisterArguments == std::nullopt)
      return std::nullopt;

    for (auto &Argument : *RegisterArguments)
      Result.Arguments.insert(Argument);

    auto Stack = tryConvertingStackArguments(Function.StackArgumentsType,
                                             RegisterArguments->size());
    if (Stack == std::nullopt)
      return std::nullopt;

    for (auto &Argument : *Stack)
      Result.Arguments.insert(Argument);

    // Convert the return type.
    const auto &GPRVR = D.GeneralPurposeReturnValueRegisters;
    const auto &RVLR = D.ReturnValueLocationRegister;
    auto ReturnValue = tryConvertingReturnValue(Function.ReturnValues,
                                                GPRVR,
                                                RVLR,
                                                *TheBinary,
                                                TypeDependencies);
    if (ReturnValue == std::nullopt)
      return std::nullopt;
    Result.ReturnType = *ReturnValue;

    // The convertion is a success, merge all the dependencies into the model.
    for (auto &&Type : TypeDependencies)
      TheBinary->recordNewType(std::move(Type));

    // Add converted type to the model.
    using UT = model::UpcastableType;
    auto Ptr = UT::make<model::CABIFunctionType>(std::move(Result));
    auto NewTypePath = TheBinary->recordNewType(std::move(Ptr));

    // Replace all references to the old type with references to the new one.
    replaceReferences(Function.key(), NewTypePath, TheBinary);

    return NewTypePath;
  }

  static model::TypePath toRaw(const model::CABIFunctionType &Function,
                               TupleTree<model::Binary> &TheBinary) {
    model::RawFunctionType Result;
    Result.CustomName = Function.CustomName;
    Result.OriginalName = Function.OriginalName;

    const abi::Definition &D = abi::predefined::get(ABI);
    auto ReturnValue = distributeReturnValue(Function.ReturnType);
    if (!Function.ReturnType.isVoid()) {
      if (!ReturnValue.Registers.empty()) {
        // Handle a register-based return value.
        for (model::Register::Values Register : ReturnValue.Registers) {
          model::TypedRegister ReturnValueRegister;
          ReturnValueRegister.Location = Register;

          TypeVector TypeDependencies;
          ReturnValueRegister.Type = chooseArgumentType(Function.ReturnType,
                                                        Register,
                                                        ReturnValue.Registers,
                                                        *TheBinary,
                                                        TypeDependencies);
          for (auto &&Dependency : TypeDependencies)
            TheBinary->recordNewType(std::move(Dependency));

          Result.ReturnValues.insert(std::move(ReturnValueRegister));
        }

        // Try and recover types from the struct if possible
        if (Function.ReturnType.Qualifiers.empty()) {
          const model::Type *Type = Function.ReturnType.UnqualifiedType.get();
          revng_assert(Type != nullptr);
          const auto *Struct = llvm::dyn_cast<model::StructType>(Type);
          if (Struct && Struct->Fields.size() == Result.ReturnValues.size()) {
            using RegisterEnum = model::Register::Values;
            SmallMap<RegisterEnum, model::QualifiedType, 4> RecoveredTypes;
            size_t StructOffset = 0;
            for (size_t Index = 0; Index < Struct->Fields.size(); ++Index) {
              if (Index >= D.GeneralPurposeReturnValueRegisters.size())
                break;
              auto Register = D.GeneralPurposeReturnValueRegisters[Index];

              auto TypedRegisterIterator = Result.ReturnValues.find(Register);
              if (TypedRegisterIterator == Result.ReturnValues.end())
                break;

              const model::StructField &Field = Struct->Fields.at(StructOffset);

              auto MaybeFieldSize = Field.Type.size();
              revng_assert(MaybeFieldSize != std::nullopt);

              auto MaybeRegisterSize = TypedRegisterIterator->Type.size();
              revng_assert(MaybeRegisterSize != std::nullopt);

              if (MaybeFieldSize.value() != MaybeRegisterSize.value())
                break;

              auto Tie = std::tie(Register, Field.Type);
              auto [Iterator, Success] = RecoveredTypes.insert(std::move(Tie));
              revng_assert(Success);

              StructOffset += MaybeFieldSize.value();
            }

            if (RecoveredTypes.size() == Result.ReturnValues.size())
              for (auto [Register, Type] : RecoveredTypes)
                Result.ReturnValues.at(Register).Type = Type;
          }
        }
      } else {
        // Handle a pointer-based return value.
        revng_assert(!D.GeneralPurposeReturnValueRegisters.empty());
        auto Register = D.GeneralPurposeReturnValueRegisters[0];
        auto RegisterSize = model::Register::getSize(Register);
        auto PointerQualifier = model::Qualifier::createPointer(RegisterSize);

        auto MaybeReturnValueSize = Function.ReturnType.size();
        revng_assert(MaybeReturnValueSize != std::nullopt);
        revng_assert(ReturnValue.Size == *MaybeReturnValueSize);

        model::QualifiedType ReturnType = Function.ReturnType;
        ReturnType.Qualifiers.emplace_back(PointerQualifier);

        model::TypedRegister ReturnPointer(Register);
        ReturnPointer.Type = std::move(ReturnType);
        Result.ReturnValues.insert(std::move(ReturnPointer));
      }
    }

    auto Arguments = distributeArguments(Function.Arguments,
                                         ReturnValue.SizeOnStack != 0);

    model::StructType StackArguments;
    uint64_t CombinedStackArgumentSize = 0;
    for (size_t ArgIndex = 0; ArgIndex < Arguments.size(); ++ArgIndex) {
      auto &ArgumentStorage = Arguments[ArgIndex];
      const auto &ArgumentType = Function.Arguments.at(ArgIndex).Type;
      if (!ArgumentStorage.Registers.empty()) {
        // Handle the registers
        auto ArgumentName = Function.Arguments.at(ArgIndex).name();
        for (size_t Index = 0; auto Register : ArgumentStorage.Registers) {
          model::NamedTypedRegister Argument(Register);

          TypeVector TypeDependencies;
          Argument.Type = chooseArgumentType(ArgumentType,
                                             Register,
                                             ArgumentStorage.Registers,
                                             *TheBinary,
                                             TypeDependencies);
          for (auto &&Dependency : TypeDependencies)
            TheBinary->recordNewType(std::move(Dependency));

          // TODO: see what can be done to preserve names better
          if (llvm::StringRef{ ArgumentName.str() }.take_front(8) != "unnamed_")
            Argument.CustomName = ArgumentName;

          Result.Arguments.insert(Argument);
        }
      }

      if (ArgumentStorage.SizeOnStack != 0) {
        // Handle the stack
        auto ArgumentIterator = Function.Arguments.find(ArgIndex);
        revng_assert(ArgumentIterator != Function.Arguments.end());
        const model::Argument &Argument = *ArgumentIterator;

        model::StructField Field;
        Field.Offset = CombinedStackArgumentSize;
        Field.CustomName = Argument.CustomName;
        Field.OriginalName = Argument.OriginalName;
        Field.Type = Argument.Type;
        StackArguments.Fields.insert(std::move(Field));

        // Compute the full size of the argument (including padding if needed).
        auto MaybeSize = Argument.Type.size();
        revng_assert(MaybeSize.has_value() && MaybeSize.value() != 0);
        CombinedStackArgumentSize += paddedSizeOnStack(MaybeSize.value());
      }
    }

    if (CombinedStackArgumentSize != 0) {
      StackArguments.Size = CombinedStackArgumentSize;

      using namespace model;
      auto Type = UpcastableType::make<StructType>(std::move(StackArguments));
      Result.StackArgumentsType = { TheBinary->recordNewType(std::move(Type)),
                                    {} };
    }

    Result.FinalStackOffset = finalStackOffset(Arguments, D);

    // Populate the list of preserved registers
    for (model::Register::Values Register : D.CalleeSavedRegisters)
      Result.PreservedRegisters.insert(Register);

    // Steal the ID
    Result.ID = Function.ID;

    // Add converted type to the model.
    using UT = model::UpcastableType;
    auto Ptr = UT::make<model::RawFunctionType>(std::move(Result));
    auto NewTypePath = TheBinary->recordNewType(std::move(Ptr));

    // Replace all references to the old type with references to the new one.
    replaceReferences(Function.key(), NewTypePath, TheBinary);

    return NewTypePath;
  }

  static uint64_t finalStackOffset(const DistributedArguments &Arguments,
                                   const abi::Definition &D) {
    constexpr auto Architecture = model::ABI::getArchitecture(ABI);
    uint64_t Result = model::Architecture::getCallPushSize(Architecture);
    if (D.CalleeIsResponsibleForStackCleanup) {
      for (auto &Argument : Arguments)
        Result += Argument.SizeOnStack;

      // TODO: take return values into the account.

      // TODO: take shadow space into the account if relevant.

      revng_assert((D.StackAlignment & (D.StackAlignment - 1)) == 0);
      Result += D.StackAlignment - 1;
      Result &= ~(D.StackAlignment - 1);
    }

    return Result;
  }

private:
  /// Takes care of extending (padding) the size of a stack argument.
  ///
  /// \note This only accounts for the post-padding (extension).
  ///       Pre-padding (offset) needs to be taken care of separately.
  ///
  /// \param RealSize The size of the argument without the padding.
  ///
  /// \return The size of the argument with the padding.
  static uint64_t paddedSizeOnStack(uint64_t RealSize) {
    if (RealSize <= RegisterSize) {
      RealSize = RegisterSize;
    } else {
      static_assert((RegisterSize & (RegisterSize - 1)) == 0);
      RealSize += RegisterSize - 1;
      RealSize &= ~(RegisterSize - 1);
    }

    return RealSize;
  }

  /// Makes sure the expected primitive type is available.
  ///
  /// If the type is not present in the binary, it's added to
  /// the \ref Dependencies.
  ///
  /// \param Kind Kind of the primitive type
  /// \param Size Size of the primitive type
  /// \param Binary Reference binary to do type lookup on
  /// \param Dependencies The container to add the type to if it's not present.
  ///
  /// \return The \ref model::QualifiedType reference to the specified type.
  static model::QualifiedType
  ensurePrimitiveTypeIsAvailable(const model::PrimitiveTypeKind::Values &Kind,
                                 std::size_t Size,
                                 const model::Binary &Binary,
                                 TypeVector &Dependencies) {
    if (auto MaybeType = Binary.tryGetPrimitiveType(Kind, Size)) {
      revng_assert(MaybeType->isValid());
      return { MaybeType.value(), {} };
    } else {
      using UT = model::UpcastableType;
      Dependencies.emplace_back(UT::make<model::PrimitiveType>(Kind, Size));
      auto Result = Binary.getTypePath(Dependencies.back().get());
      revng_assert(!Result.path().empty());
      return { Binary.getTypePath(Dependencies.back().get()), {} };
    }
  }

  /// Checks if the input type is valid.
  ///
  /// If the type is not present in the binary, it's added to
  /// the \ref Dependencies.
  ///
  /// \param Type The type to be checked.
  /// \param Register The register to pull value size from.
  /// \param Binary Reference binary to do type lookup on.
  /// \param Dependencies The container to add the type to if it's not present.
  ///
  /// \return
  static model::QualifiedType
  ensureTypeIsAvailable(const model::QualifiedType &Type,
                        model::Register::Values Register,
                        const model::Binary &Binary,
                        TypeVector &Dependencies) {
    const auto Kind = model::Register::primitiveKind(Register);
    const std::size_t Size = model::Register::getSize(Register);
    if (Type.UnqualifiedType.get() != nullptr)
      return Type;
    else
      return ensurePrimitiveTypeIsAvailable(Kind, Size, Binary, Dependencies);
  }

  /// Helper used for converting register arguments to the c-style
  /// representation
  ///
  /// \param Used a set of registers confirmed to be in use by the function in
  ///        question.
  /// \param Allowed an ordered list of registers allowed to be used for passing
  ///        parameters by the ABI.
  /// \param Binary a const reference to the binary containing information
  ///        about already present types.
  /// \param TypeDependencies a list of types that need to be added to the model
  ///        for all the returned arguments to be valid.
  ///
  /// \tparam RegisterType type of a register, should be deduced from
  ///         the \ref Used.
  ///
  /// \return an optional list of arguments or `std::nullopt` if the conversion
  ///         was not successful.
  template<typename RegisterType>
  static std::optional<llvm::SmallVector<model::Argument, 8>>
  tryConvertingRegisterArguments(const SortedVector<RegisterType> &Used,
                                 const RegisterVector &Allowed,
                                 const model::Binary &Binary,
                                 TypeVector &TypeDependencies) {
    llvm::SmallVector<model::Argument, 8> Result;

    // A flag used to differentiate state of the analysis. It is set to `true`,
    // if there is at least one used register to the right of the current
    // one making it very likely to be either used or skipped over as padding.
    bool MustUseTheNextOne = false;

    // Enumerate all the registers dedicated by the ABI for passing parameters
    // in reverse order (last register is looked at first).
    for (auto Pair : llvm::enumerate(llvm::reverse(Allowed))) {
      size_t Index = Allowed.size() - Pair.index() - 1;
      model::Register::Values Register = Pair.value();
      auto CurrentRegisterIterator = Used.find(Register);
      if (CurrentRegisterIterator != Used.end()) {
        // If the current register is confirmed to be in use, convert it into
        // an argument.
        auto &Current = Result.emplace_back();
        Current.Type = ensureTypeIsAvailable(Used.at(Register).Type,
                                             Register,
                                             Binary,
                                             TypeDependencies);
        Current.CustomName = Used.at(Register).CustomName;
      } else if (MustUseTheNextOne) {
        // If at least one register to the right of the current one is confirmed
        // to be in use

        // Specifically handle the case where the ABI defines explicit rules for
        // passing a single argument in a pair of registers
        bool WasHandledSpecifically = false;
        const abi::Definition &D = abi::predefined::get(ABI);
        if (D.OnlyStartDoubleArgumentsFromAnEvenRegister) {
          // Only applicable for oddly-numbered registers (2nd, 4th, etc)
          // except for the 0th.
          if (Index != 0 && (Index & 1) == 0) {
            // There had to be at least two valid registers added to
            // the `Result` already
            if (Result.size() > 1) {
              auto UpperRegister = Allowed.at(Index - 2);
              auto LowerRegister = Allowed.at(Index - 1);

              // Both registers should be of the same kind for them to be merged
              auto UpperKind = model::Register::primitiveKind(UpperRegister);
              auto LowerKind = model::Register::primitiveKind(LowerRegister);
              if (UpperKind == LowerKind) {
                auto &Upper = Result[Result.size() - 2];
                auto &Lower = Result[Result.size() - 1];

                // TODO: see what can be done to preserve names better
                if (Upper.CustomName.empty() && !Lower.CustomName.empty())
                  Upper.CustomName = Lower.CustomName;

                // Merge two registers into a single argument.
                size_t Size = model::Register::getSize(UpperRegister)
                              + model::Register::getSize(LowerRegister);
                Upper.Type = ensurePrimitiveTypeIsAvailable(UpperKind,
                                                            Size,
                                                            Binary,
                                                            TypeDependencies);

                // Drop the merged half of the current type from the list.
                Result.pop_back();

                WasHandledSpecifically = true;
              }
            }
          }
        }

        if (!WasHandledSpecifically) {
          // TODO: It might still be worth it to add this unused register as
          // an argument to indicate the register ordering.
        }
      } else {
        // Ignore this register, it's most like unused.
      }

      bool IsUsed = (CurrentRegisterIterator != Used.end());
      MustUseTheNextOne = MustUseTheNextOne || IsUsed;
    }

    // Ensure all the indices are consistent.
    for (auto Pair : llvm::enumerate(llvm::reverse(Result)))
      Pair.value().Index = Pair.index();

    return Result;
  }

  /// Helper used for converting stack argument struct into
  /// the c-style representation
  ///
  /// \param StackArgumentTypes The qualified type of the relevant part of
  ///        the stack.
  /// \param IndexOffset The index of the first argument (indicates the register
  ///        argument count).
  ///
  /// \return An ordered list of arguments.
  static std::optional<llvm::SmallVector<model::Argument, 8>>
  tryConvertingStackArguments(model::QualifiedType StackArgumentTypes,
                              size_t IndexOffset) {
    // Qualifiers here are not allowed since this must point to a simple struct.
    revng_assert(StackArgumentTypes.Qualifiers.empty());

    auto *Unqualified = StackArgumentTypes.UnqualifiedType.get();
    if (!Unqualified) {
      // If there is no type, it means that the importer responsible for this
      // function didn't detect any stack arguments and avoided creating
      // a new empty type.
      return llvm::SmallVector<model::Argument, 8>{};
    }

    auto *Pointer = llvm::dyn_cast<model::StructType>(Unqualified);
    revng_assert(Pointer != nullptr,
                 "`RawFunctionType::StackArgumentsType` must be a struct");
    const model::StructType &Types = *Pointer;

    // If the struct is empty, there's no arguments to extract from it.
    if (Types.Fields.empty()) {
      revng_assert(Types.size() == 0);
      return llvm::SmallVector<model::Argument, 8>{};
    }

    // Compute the alignment of the first argument.
    auto CurrentAlignment = *abi::naturalAlignment(Types.Fields.begin()->Type,
                                                   ABI);
    if ((CurrentAlignment & (CurrentAlignment - 1)) == 0) {
      // Abandon cases where alignment is equal to 0 or is not a power of two.
      return std::nullopt;
    }

    // Process each of the fields, converting them into arguments separately.
    llvm::SmallVector<model::Argument, 8> Result;
    for (auto Iterator = Types.Fields.begin();
         Iterator != std::prev(Types.Fields.end());
         ++Iterator) {
      const auto &CurrentArgument = *Iterator;
      auto MaybeSize = CurrentArgument.Type.size();
      revng_assert(MaybeSize.has_value() && MaybeSize.value() != 0);
      uint64_t CurrentSize = paddedSizeOnStack(MaybeSize.value());

      const auto &NextArgument = *std::next(Iterator);
      auto NextAlignment = *abi::naturalAlignment(NextArgument.Type, ABI);
      if ((CurrentAlignment & (CurrentAlignment - 1)) == 0) {
        // Abandon cases where alignment is equal to 0 or is not a power of two.
        return std::nullopt;
      }

      if (CurrentArgument.Offset + CurrentSize != NextArgument.Offset
          || (CurrentArgument.Offset + CurrentSize) % NextAlignment != 0) {
        // Abandon any cases where natural alignment causes a padding "hole"
        // to be required.
        //
        // TODO: we should probably preprocess the input to fill such holes in
        //       before trying to convert the functions.
        return std::nullopt;
      }

      // Create the argument from this field.
      model::Argument &New = Result.emplace_back();
      New.Index = IndexOffset++;
      New.Type = CurrentArgument.Type;
      New.CustomName = CurrentArgument.CustomName;
      New.OriginalName = CurrentArgument.OriginalName;

      CurrentAlignment = NextAlignment;
    }

    return Result;
  }

  /// Helper used for converting return value representation to
  /// the c-style
  ///
  /// \param Used a set of registers confirmed to be in use by
  ///        the function in question.
  /// \param Allowed an ordered list of registers allowed to be used
  ///        for returning values by the ABI.
  /// \param PointerToCopyLocation an optional register used for pointing to
  ///        return values in memory on some architectures. It is set to
  ///        `model::Register::Invalid` if it's not applicable.
  /// \param Binary a const reference to the binary containing information
  ///        about already present types.
  /// \param TypeDependencies a list of types that need to be added to the model
  ///        for the returned return value to be valid.
  ///
  /// \tparam RegisterType type of a register, should be deduced from
  ///         the \ref Used.
  ///
  /// \return
  template<typename RegisterType, bool DryRun = false>
  static std::optional<model::QualifiedType>
  tryConvertingReturnValue(const SortedVector<RegisterType> &Used,
                           const RegisterVector &Allowed,
                           const model::Register::Values PointerToCopyLocation,
                           const model::Binary &Binary,
                           TypeVector &TypeDependencies) {
    if (Used.size() == 0) {
      // Short-circuit the functions that don't return anything (`void`).
      return ensurePrimitiveTypeIsAvailable(model::PrimitiveTypeKind::Void,
                                            0,
                                            Binary,
                                            TypeDependencies);
    }

    if (Allowed.empty()) {
      // A return value is expected on an ABI that doesn't dedicate any
      // registers to be used for those.
      return std::nullopt;
    }

    // Special case where the only used register is the pointer-to-copy location
    if (Used.size() == 1 && Used.begin()->Location == PointerToCopyLocation) {
      if (Used.begin()->Type.isPointer()) {
        // If the type is already a pointer, preserve it.
        return Used.begin()->Type;
      } else {
        // Otherwise return a `void *` pointer as the return value type.
        static constexpr auto VoidKind = model::PrimitiveTypeKind::Void;
        auto Result = ensurePrimitiveTypeIsAvailable(VoidKind,
                                                     0,
                                                     Binary,
                                                     TypeDependencies);
        using model::Qualifier;
        Result.Qualifiers.emplace_back(Qualifier::createPointer(Architecture));
        return Result;
      }
    }

    // TODO: there are some cases where preserving the total structure of
    // the return value can be benefitial, this needs more work.
    // For now, just determine the total size of the return value and if it
    // fits into a primitive value - emit it, otherwise show those values as
    // a struct (/note: positional ABIs need a whole lot of extra love in this
    // regard).
    llvm::SmallVector<std::optional<model::QualifiedType>, 4> ResultingTypes;
    bool MustUseTheNextOne = false;
    auto AllowedRange = llvm::enumerate(llvm::reverse(Allowed));
    for (auto Pair : AllowedRange) {
      size_t Index = Allowed.size() - Pair.index() - 1;
      model::Register::Values Register = Pair.value();
      auto UsedIterator = Used.find(Register);
      if (UsedIterator != Used.end())
        ResultingTypes.emplace_back(UsedIterator->Type);
      else if (MustUseTheNextOne)
        ResultingTypes.emplace_back(std::nullopt);

      MustUseTheNextOne = MustUseTheNextOne || (UsedIterator != Used.end());
    }

    revng_assert(ResultingTypes.size() != 0);
    if (ResultingTypes.size() == 1) {
      // Short-circuit for a single register return value.
      auto Kind = model::Register::primitiveKind(Allowed.at(0));
      if (ResultingTypes.front() != std::nullopt) {
        return ensureTypeIsAvailable(*ResultingTypes.front(),
                                     Allowed.at(0),
                                     Binary,
                                     TypeDependencies);
      } else {
        // No information about the type of this register, use the default.
        return ensureTypeIsAvailable(model::QualifiedType{},
                                     Allowed.at(0),
                                     Binary,
                                     TypeDependencies);
      }
    } else if (ResultingTypes.size() == 2) {
      // Short-circuit for a double register return value.
      if (Allowed.size() < 2) {
        // The ABI doesn't support double register return values.
        // There's a good chance the ABI was not detected correctly.
        return std::nullopt;
      }

      auto UpperKind = model::Register::primitiveKind(Allowed.at(0));
      auto LowerKind = model::Register::primitiveKind(Allowed.at(1));
      if (UpperKind != LowerKind)
        return std::nullopt;

      // Merge two registers together.
      size_t UpperSize = model::Register::getSize(Allowed.at(0));
      size_t LowerSize = model::Register::getSize(Allowed.at(1));
      return ensurePrimitiveTypeIsAvailable(UpperKind,
                                            UpperSize + LowerSize,
                                            Binary,
                                            TypeDependencies);
    } else {
      // Make struct for now, but there should be better options.
      model::UpcastableType Result = model::makeType<model::StructType>();
      auto ReturnStruct = llvm::dyn_cast<model::StructType>(Result.get());
      for (const auto &MaybeType : ResultingTypes) {
        model::QualifiedType Qualified;
        if (MaybeType.has_value())
          Qualified = MaybeType.value();

        model::StructField CurrentField;
        CurrentField.Offset = ReturnStruct->Size;
        CurrentField.Type = ensureTypeIsAvailable(Qualified,
                                                  Allowed.at(0),
                                                  Binary,
                                                  TypeDependencies);
        ReturnStruct->Fields.insert(std::move(CurrentField));

        auto MaybeSize = CurrentField.Type.size();
        revng_assert(MaybeSize.has_value());

        ReturnStruct->Size += MaybeSize.value();
      }

      revng_assert(ReturnStruct->Size != 0 && !ReturnStruct->Fields.empty());

      model::QualifiedType QualifiedResult(Binary.getTypePath(Result.get()),
                                           {});
      TypeDependencies.emplace_back(std::move(Result));
      return QualifiedResult;
    }

    return std::nullopt;
  }

  static DistributedArguments
  distributePositionBasedArguments(const ArgumentVector &Arguments,
                                   const abi::Definition &D,
                                   std::size_t SkippedRegisters = 0) {
    DistributedArguments Result;

    for (const model::Argument &Argument : Arguments) {
      std::size_t RegisterIndex = Argument.Index + SkippedRegisters;
      if (Result.size() <= RegisterIndex)
        Result.resize(RegisterIndex + 1);
      auto &Distributed = Result[RegisterIndex];

      auto MaybeSize = Argument.Type.size();
      revng_assert(MaybeSize.has_value());
      Distributed.Size = *MaybeSize;

      if (Argument.Type.isFloat()) {
        if (RegisterIndex < D.VectorArgumentRegisters.size()) {
          auto Register = D.VectorArgumentRegisters[RegisterIndex];
          Distributed.Registers.emplace_back(Register);
        } else {
          Distributed.SizeOnStack = Distributed.Size;
        }
      } else {
        if (RegisterIndex < D.GeneralPurposeArgumentRegisters.size()) {
          auto Reg = D.GeneralPurposeArgumentRegisters[RegisterIndex];
          Distributed.Registers.emplace_back(Reg);
        } else {
          Distributed.SizeOnStack = Distributed.Size;
        }
      }
    }

    return Result;
  }

  static constexpr auto UnlimitedRegisters = std::numeric_limits<size_t>::max();

  static std::pair<DistributedArgument, size_t>
  considerRegisters(size_t Size,
                    size_t AllowedRegisterLimit,
                    size_t OccupiedRegisterCount,
                    const RegisterVector &AllowedRegisters,
                    bool AllowPuttingPartOfAnArgumentOnStack) {
    size_t RegisterLimit = OccupiedRegisterCount + AllowedRegisterLimit;
    size_t ConsideredRegisterCounter = OccupiedRegisterCount;

    size_t SizeCounter = 0;
    const size_t ARC = AllowedRegisters.size();
    if (ARC > 0) {
      size_t &CRC = ConsideredRegisterCounter;
      while (SizeCounter < Size && CRC < ARC && CRC < RegisterLimit) {
        size_t RegisterIndex = ConsideredRegisterCounter++;
        auto CurrentRegister = AllowedRegisters[RegisterIndex];
        SizeCounter += model::Register::getSize(CurrentRegister);
      }
    }

    DistributedArgument DA;
    DA.Size = Size;

    const abi::Definition &D = abi::predefined::get(ABI);
    if (D.OnlyStartDoubleArgumentsFromAnEvenRegister) {
      if (ConsideredRegisterCounter - OccupiedRegisterCount == 2) {
        if ((OccupiedRegisterCount & 1) != 0) {
          ++OccupiedRegisterCount;
          ++ConsideredRegisterCounter;
        }
      }
    }

    if (SizeCounter >= Size) {
      for (size_t I = OccupiedRegisterCount; I < ConsideredRegisterCounter; ++I)
        DA.Registers.emplace_back(AllowedRegisters[I]);
      DA.SizeOnStack = 0;
    } else if (AllowPuttingPartOfAnArgumentOnStack) {
      for (size_t I = OccupiedRegisterCount; I < ConsideredRegisterCounter; ++I)
        DA.Registers.emplace_back(AllowedRegisters[I]);
      DA.SizeOnStack = DA.Size - SizeCounter;
    } else {
      DA.SizeOnStack = DA.Size;
      ConsideredRegisterCounter = OccupiedRegisterCount;
    }

    if (DA.SizeOnStack != 0)
      DA.SizeOnStack = paddedSizeOnStack(DA.SizeOnStack);

    return { DA, ConsideredRegisterCounter };
  }

  static DistributedArguments
  distributeNonPositionBasedArguments(const ArgumentVector &Arguments,
                                      const abi::Definition &D,
                                      std::size_t SkippedRegisters = 0) {
    DistributedArguments Result;
    size_t UsedGeneralPurposeRegisterCounter = SkippedRegisters;
    size_t UsedVectorRegisterCounter = 0;

    for (const model::Argument &Argument : Arguments) {
      auto MaybeSize = Argument.Type.size();
      revng_assert(MaybeSize.has_value());

      const bool CanSplit = D.ArgumentsCanBeSplitBetweenRegistersAndStack;
      if (Argument.Type.isFloat()) {
        const auto &Registers = D.VectorArgumentRegisters;
        size_t &Counter = UsedVectorRegisterCounter;
        const size_t Limit = 1;

        auto [Distributed, NextIndex] = considerRegisters(*MaybeSize,
                                                          Limit,
                                                          Counter,
                                                          Registers,
                                                          CanSplit);
        if (Result.size() <= Argument.Index)
          Result.resize(Argument.Index + 1);
        Result[Argument.Index] = Distributed;
        Counter = NextIndex;
      } else {
        const auto &Registers = D.GeneralPurposeArgumentRegisters;
        size_t &Counter = UsedGeneralPurposeRegisterCounter;
        if (Argument.Type.isScalar()) {
          const size_t Limit = D.MaximumGPRsPerScalarArgument;

          auto [Distributed, NextIndex] = considerRegisters(*MaybeSize,
                                                            Limit,
                                                            Counter,
                                                            Registers,
                                                            CanSplit);
          if (Result.size() <= Argument.Index)
            Result.resize(Argument.Index + 1);
          Result[Argument.Index] = Distributed;
          Counter = NextIndex;
        } else {
          const size_t Limit = D.MaximumGPRsPerAggregateArgument;

          auto [Distributed, NextIndex] = considerRegisters(*MaybeSize,
                                                            Limit,
                                                            Counter,
                                                            Registers,
                                                            CanSplit);
          if (Result.size() <= Argument.Index)
            Result.resize(Argument.Index + 1);
          Result[Argument.Index] = Distributed;
          Counter = NextIndex;
        }
      }
    }

    return Result;
  }

public:
  static DistributedArguments
  distributeArguments(const ArgumentVector &Arguments,
                      bool PassesReturnValueLocationAsAnArgument) {
    bool SkippedCount = 0;

    const abi::Definition &D = abi::predefined::get(ABI);
    if (PassesReturnValueLocationAsAnArgument == true) {
      revng_assert(verifyReturnValueLocationHelper(D),
                   "ABIs where non-first argument GPR is used for passing the "
                   "address of the space allocated for big return values are "
                   "not currently supported.");

      const auto &GPRs = D.GeneralPurposeArgumentRegisters;
      if (!GPRs.empty() && D.ReturnValueLocationRegister == GPRs[0])
        SkippedCount = 1;
    }

    if (D.ArgumentsArePositionBased)
      return distributePositionBasedArguments(Arguments, D, SkippedCount);
    else
      return distributeNonPositionBasedArguments(Arguments, D, SkippedCount);
  }

  static DistributedArgument
  distributeReturnValue(const model::QualifiedType &ReturnValueType) {
    if (ReturnValueType.isVoid())
      return DistributedArgument{};

    auto MaybeSize = ReturnValueType.size();
    revng_assert(MaybeSize.has_value());

    const abi::Definition &D = abi::predefined::get(ABI);
    if (ReturnValueType.isFloat()) {
      const auto &Registers = D.VectorReturnValueRegisters;
      // TODO: replace `UnlimitedRegisters` with the actual value to be defined
      //       by the trait.
      const size_t L = UnlimitedRegisters;
      return considerRegisters(*MaybeSize, L, 0, Registers, false).first;
    } else {
      const size_t L = ReturnValueType.isScalar() ?
                         D.MaximumGPRsPerScalarReturnValue :
                         D.MaximumGPRsPerAggregateReturnValue;
      const auto &Registers = D.GeneralPurposeReturnValueRegisters;
      return considerRegisters(*MaybeSize, L, 0, Registers, false).first;
    }
  }

private:
  static model::QualifiedType
  chooseArgumentType(const model::QualifiedType &ArgumentType,
                     model::Register::Values Register,
                     const RegisterList &RegisterList,
                     model::Binary &Binary,
                     TypeVector &Dependencies) {
    if (RegisterList.size() > 1) {
      // TODO: this can be considerably improved, we can preserve more
      //       information here over just labeling it as `Generic`.
      return ensurePrimitiveTypeIsAvailable(model::PrimitiveTypeKind::Generic,
                                            model::Register::getSize(Register),
                                            Binary,
                                            Dependencies);
    } else {
      auto ResultType = ArgumentType;
      auto MaybeSize = ResultType.size();
      auto TargetSize = model::Register::getSize(Register);

      if (!MaybeSize.has_value()) {
        using namespace model;
        return ensurePrimitiveTypeIsAvailable(Register::primitiveKind(Register),
                                              Register::getSize(Register),
                                              Binary,
                                              Dependencies);
      } else if (*MaybeSize > TargetSize) {
        // TODO: some considerations should be made for position-based ABIs here
        auto Qualifier = model::Qualifier::createPointer(TargetSize);
        ResultType.Qualifiers.emplace_back(Qualifier);
      } else if (!ResultType.isScalar()) {
        // TODO: this can be considerably improved, we can preserve more
        //       information here over just labeling it as `Generic`.
        const auto Size = model::Register::getSize(Register);
        return ensurePrimitiveTypeIsAvailable(model::PrimitiveTypeKind::Generic,
                                              Size,
                                              Binary,
                                              Dependencies);
      }

      return ResultType;
    }
  }
};

std::optional<model::TypePath>
tryConvertToCABI(const model::RawFunctionType &Function,
                 TupleTree<model::Binary> &TheBinary,
                 std::optional<model::ABI::Values> MaybeABI) {
  if (!MaybeABI.has_value())
    MaybeABI = TheBinary->DefaultABI;
  revng_assert(*MaybeABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(*MaybeABI, [&]<model::ABI::Values A>() {
    return ConversionHelper<A>::toCABI(Function, TheBinary);
  });
}

model::TypePath convertToRaw(const model::CABIFunctionType &Function,
                             TupleTree<model::Binary> &TheBinary) {
  revng_assert(Function.ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(Function.ABI, [&]<model::ABI::Values A>() {
    return ConversionHelper<A>::toRaw(Function, TheBinary);
  });
}

Layout::Layout(const model::CABIFunctionType &Function) :
  Layout(skippingEnumSwitch<1>(Function.ABI, [&]<model::ABI::Values A>() {
    Layout Result;

    const abi::Definition &D = abi::predefined::get(A);
    auto RV = ConversionHelper<A>::distributeReturnValue(Function.ReturnType);
    if (RV.SizeOnStack == 0) {
      // Nothing on the stack, the return value fits into the registers.
      Result.ReturnValue.Registers = std::move(RV.Registers);
      Result.ReturnValue.Type = Function.ReturnType;
    } else {
      revng_assert(RV.Registers.empty(),
                   "Register and stack return values should never be present "
                   "at the same time.");

      revng_assert(D.ReturnValueLocationRegister != model::Register::Invalid,
                   "Big return values are not supported by the current ABI");
      auto &RVLocationArg = Result.Arguments.emplace_back();
      RVLocationArg.Registers.emplace_back(D.ReturnValueLocationRegister);

      static constexpr auto Architecture = model::ABI::getArchitecture(A);
      RVLocationArg.Type = Function.ReturnType.getPointerTo(Architecture);
    }

    size_t CurrentOffset = 0;
    auto Args = ConversionHelper<A>::distributeArguments(Function.Arguments,
                                                         RV.SizeOnStack != 0);
    revng_assert(Args.size() == Function.Arguments.size());
    for (size_t Index = 0; Index < Args.size(); ++Index) {
      auto &Current = Result.Arguments.emplace_back();
      Current.Type = Function.Arguments.at(Index).Type;
      Current.Registers = std::move(Args[Index].Registers);
      if (Args[Index].SizeOnStack != 0) {
        // TODO: further alignment considerations are needed here.
        Current.Stack = typename Layout::Argument::StackSpan{
          CurrentOffset, Args[Index].SizeOnStack
        };
        CurrentOffset += Args[Index].SizeOnStack;
      }
    }

    Result.CalleeSavedRegisters.resize(D.CalleeSavedRegisters.size());
    llvm::copy(D.CalleeSavedRegisters, Result.CalleeSavedRegisters.begin());

    Result.FinalStackOffset = ConversionHelper<A>::finalStackOffset(Args, D);

    return Result;
  })) {
}

Layout::Layout(const model::RawFunctionType &Function) {
  // Lay register arguments out.
  for (const model::NamedTypedRegister &Register : Function.Arguments) {
    Arguments.emplace_back().Registers = { Register.Location };
    Arguments.back().Type = Register.Type;
  }

  // Lay the return value out.
  for (const model::TypedRegister &Register : Function.ReturnValues) {
    ReturnValue.Registers.emplace_back(Register.Location);
    ReturnValue.Type = Register.Type;
  }

  // Lay stack arguments out.
  if (Function.StackArgumentsType.UnqualifiedType.isValid()) {
    revng_assert(Function.StackArgumentsType.Qualifiers.empty());
    const model::Type *OriginalStackType = Function.StackArgumentsType
                                             .UnqualifiedType.get();
    auto *StackStruct = llvm::dyn_cast<model::StructType>(OriginalStackType);
    revng_assert(StackStruct,
                 "`RawFunctionType::StackArgumentsType` must be a struct.");
    typename Layout::Argument::StackSpan StackSpan{ 0, StackStruct->Size };
    Arguments.emplace_back().Stack = std::move(StackSpan);
    Arguments.back().Type = Function.StackArgumentsType;
  }

  // Fill callee saved registers.
  append(Function.PreservedRegisters, CalleeSavedRegisters);

  // Set the final offset.
  FinalStackOffset = Function.FinalStackOffset;
}

bool Layout::verify() const {
  model::Architecture::Values ExpectedArch = model::Architecture::Invalid;
  std::unordered_set<model::Register::Values> LookupHelper;
  auto VerificationHelper = [&](model::Register::Values Register) -> bool {
    // Ensure each register is present only once
    if (!LookupHelper.emplace(Register).second)
      return false;

    // Ensure all the registers belong to the same architecture
    if (ExpectedArch == model::Architecture::Invalid)
      ExpectedArch = model::Register::getArchitecture(Register);
    else if (ExpectedArch != model::Register::getArchitecture(Register))
      return false;

    return true;
  };

  // Verify arguments
  LookupHelper.clear();
  for (const Layout::Argument &Argument : Arguments)
    for (model::Register::Values Register : Argument.Registers)
      if (!VerificationHelper(Register))
        return false;

  // Verify return values
  LookupHelper.clear();
  for (model::Register::Values Register : ReturnValue.Registers)
    if (!VerificationHelper(Register))
      return false;

  return true;
}

size_t Layout::argumentRegisterCount() const {
  size_t Result = 0;

  for (auto &Argument : Arguments)
    Result += Argument.Registers.size();

  return Result;
}

size_t Layout::returnValueRegisterCount() const {
  return ReturnValue.Registers.size();
}

llvm::SmallVector<model::Register::Values, 8>
Layout::argumentRegisters() const {
  llvm::SmallVector<model::Register::Values, 8> Result;

  for (auto &Argument : Arguments)
    Result.append(Argument.Registers.begin(), Argument.Registers.end());

  return Result;
}

llvm::SmallVector<model::Register::Values, 8>
Layout::returnValueRegisters() const {
  return llvm::SmallVector<model::Register::Values,
                           8>(ReturnValue.Registers.begin(),
                              ReturnValue.Registers.end());
}

} // namespace abi::FunctionType
