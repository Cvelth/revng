/// \file FunctionType.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <concepts>
#include <unordered_set>

#include "llvm/ADT/SmallSet.h"

#include "revng/ABI/FunctionType.h"
#include "revng/ABI/ModelAlignment.h"
#include "revng/ABI/Predefined.h"
#include "revng/ABI/RegisterOrder.h"
#include "revng/ABI/RegisterStateDeductions.h"
#include "revng/ADT/Concepts.h"
#include "revng/ADT/STLExtras.h"
#include "revng/ADT/SmallMap.h"
#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/VerifyHelper.h"
#include "revng/Support/EnumSwitch.h"

static Logger Log("abi-function-type-conversion");

namespace abi::FunctionType {

using RegisterVector = std::vector<model::Register::Values>;

/// A helper producing the most obvious register checks, it's used by the
/// `toCABI` side of the conversion to determine whether it's possible.
///
/// @tparam Architecture The architecture of the binary. It's used for checking
///         for rogue registers.
/// @tparam RegisterType The representation of the register. It's templated
///         to allow both `TypedRegister` and `NamedTypedRegister` to be passed
///         in.
///
/// @param UsedRegisters The list of the registers confirmed to be used by the
///        function. It should be extracted from the `RawFunctionType`.
/// @param AllowedRegisters The list of the registers allowed to be used by the
///        specific ABI. It should be extracted from the trait.
///
/// @return `true` if conversion should attempted, `false` otherwise.
template<model::Architecture::Values Architecture, typename RegisterType>
bool verify(const SortedVector<RegisterType> &UsedRegisters,
            const RegisterVector &AllowedRegisters) {
  for (const model::Register::Values &Register : AllowedRegisters) {
    // Check if architecture of the allowed registers makes sense.
    if (model::Register::getArchitecture(Register) != Architecture)
      revng_abort();

    // Check if there are any duplicates in the allowed register list.
    // This would mean that the trait is ill-formed.
    if (llvm::count(AllowedRegisters, Register) != 1)
      revng_abort();
  }

  for (const RegisterType &Register : UsedRegisters) {
    // Check if architecture of the used registers makes sense.
    if (model::Register::getArchitecture(Register.Location) != Architecture)
      revng_abort();
  }

  // Check for any used registers that are not allowed.
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
constexpr bool isVectorRegister(model::Register::Values Register) {
  namespace PrimitiveTypeKind = model::PrimitiveTypeKind;
  return model::Register::primitiveKind(Register) == PrimitiveTypeKind::Float;
}

/// Helps detecting unsupported ABI trait definition with respect to
/// the way they return the return values.
///
/// This is an important piece of abi trait verification. For more information
/// see the `static_assert` that invokes it in \ref distributeArguments
///
/// \return `true` if the ABI is valid, `false` otherwise.
static bool verifyReturnValueLocation(const abi::Definition &D) {
  // Skip ABIs that do not allow returning big values.
  // They do not benefit from this check.
  if (D.ReturnValueLocationRegister != model::Register::Invalid) {
    if (isVectorRegister(D.ReturnValueLocationRegister)) {
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

/// Replace all the references to `OldKey` with the references to
/// the newly added `NewType`. It also erases the old type.
///
/// \param OldKey The type references of which should be replaced.
/// \param NewType The new type to replace the references to. It is added to
///        the model implicitly, no need to do so before calling this function.
/// \param Model The tuple tree where replacement should take place in.
///
/// \return The new path to the added type.
static model::TypePath replaceAllUsesWith(const model::Type::Key &OldKey,
                                          const model::TypePath &NewTypePath,
                                          TupleTree<model::Binary> &Model) {
  auto Visitor = [&NewTypePath, &OldKey](model::TypePath &Visited) {
    if (!Visited.isValid())
      return; // Ignore empty references

    const model::Type *Current = Visited.getConst();
    revng_assert(Current != nullptr);

    if (Current->key() == OldKey)
      Visited = NewTypePath;
  };
  Model.visitReferences(Visitor);

  Model->Types.erase(OldKey);

  return NewTypePath;
}

/// A helper used to simplify selecting types for registers.
///
/// \param Register Any CPU register the model is aware of.
/// \param Binary A model to obtain the type from.
///
/// \return A primitive type in \ref Binary.
static model::TypePath
defaultRegisterType(model::Register::Values Register, model::Binary &Binary) {
  return Binary.getPrimitiveType(model::Register::primitiveKind(Register),
                                 model::Register::getSize(Register));
}

/// A helper used to simplify selecting types for registers.
///
/// \param Register Any CPU register the model is aware of.
/// \param Binary A model to obtain the type from.
///
/// \return A primitive type in \ref Binary.
static model::TypePath
genericRegisterType(model::Register::Values Register, model::Binary &Binary) {
  return Binary.getPrimitiveType(model::PrimitiveTypeKind::Generic,
                                 model::Register::getSize(Register));
}

template<typename Type>
requires integral<std::decay_t<Type>>
bool IsZeroOrPowerOfTwo(Type &&Value) {
  return Value & (Value - 1) == 0;
}

/// Takes care of extending (padding) the size of a stack argument.
///
/// \note This only accounts for the post-padding (extension).
///       Pre-padding (offset) needs to be taken care of separately.
///
/// \param RealSize The size of the argument without the padding.
/// \param RegisterSize The size of a register under the given architecture.
///
/// \return The size of the argument with the padding.
static uint64_t paddedSizeOnStack(uint64_t RealSize, uint64_t RegisterSize) {
  if (RealSize <= RegisterSize)
    return RegisterSize;

  revng_assert(IsZeroOrPowerOfTwo(RegisterSize));
  RealSize += RegisterSize - 1;
  RealSize &= ~(RegisterSize - 1);
  return RealSize;
}

namespace ModelArch = model::Architecture;

struct Converter {
public:
  using RegisterList = llvm::SmallVector<model::Register::Values, 1>;
  struct DistributedArgument {
    RegisterList Registers = {};
    size_t Size = 0, SizeOnStack = 0;
  };
  using DistributedArguments = llvm::SmallVector<DistributedArgument, 4>;

  using ArgumentVector = SortedVector<model::Argument>;

public:
  const abi::Definition &ABI;

public:
  explicit Converter(model::ABI::Values ABI) : ABI(abi::predefined::get(ABI)) {}

public:
  /// Entry point for the `toCABI` conversion.
  std::optional<model::TypePath> toCABI(const model::RawFunctionType &Function,
                                        TupleTree<model::Binary> &Binary) const;

  /// Entry point for the `toRaw` conversion.
  model::TypePath toRaw(const model::CABIFunctionType &Function,
                        TupleTree<model::Binary> &Binary) const;

public:
  struct TypeDependencyManagementHelper {
  private:
    const abi::Definition &ABI;
    model::Binary &Binary;

    using TypeVector = llvm::SmallVector<UpcastablePointer<model::Type>, 16>;
    TypeVector Dependencies;

  private:
    friend Converter;
    explicit TypeDependencyManagementHelper(const abi::Definition &ABI,
                                            model::Binary &Binary) :
      ABI(ABI), Binary(Binary) {}
    ~TypeDependencyManagementHelper() { commit(); }

  public:
    /// Marks all the current dependencies as essential and merges them into
    /// the managed model.
    ///
    /// The managed model is the one \ref Binary points to.
    void commit() {
      for (auto &&Dependency : Dependencies)
        Binary.recordNewType(std::move(Dependency));
      Dependencies.clear();
    }

    /// Aborts dependency management and discards any modification to
    /// the managed model made by member methods until this point.
    ///
    /// The managed model is the one \ref Binary points to.
    void drop() { Dependencies.clear(); }

  public:
    /// Helper used for converting register arguments to the c-style
    /// representation
    ///
    /// \param Used a set of registers confirmed to be in use by the function in
    ///        question.
    ///
    /// \tparam RegType type of a register. It should always be deduced from
    ///         the \ref Used.
    ///
    /// \return a list of arguments if the conversion was successful,
    ///         `std::nullopt` otherwise.
    template<typename RegType>
    std::optional<llvm::SmallVector<model::Argument, 8>>
    tryConvertingRegisterArguments(const SortedVector<RegType> &Used);

    /// Helper used for converting return values to the c-style representation
    ///
    /// \param Used a set of registers confirmed to be in use by
    ///        the function in question.
    /// \param Allowed an ordered list of registers allowed to be used
    ///        for returning values by the ABI.
    /// \param PtrToCopyLocation an optional register used for pointing to
    ///        return values in memory on some architectures. It is set to
    ///        `model::Register::Invalid` if it's not applicable.
    ///
    /// \tparam RegisterType type of a register, should be deduced from
    ///         the \ref Used.
    ///
    /// \return a qualified type if conversion is possible, `std::nullopt`
    ///         otherwise.
    template<typename RegisterType>
    std::optional<model::QualifiedType>
    tryConvertingReturnValue(const SortedVector<RegisterType> &UsedRegisters);

  public:
    /// Helper for choosing the "Raw" type of a specific argument or return
    /// value based on its "C" type and the register it is to be placed in.
    ///
    /// \param ArgumentType The original "C" type.
    /// \param Register Register this value is placed in.
    /// \return The resulting "Raw" type for the current entity.
    model::QualifiedType chooseType(const model::QualifiedType &ArgumentType,
                                    const model::Register::Values Register);
  };

  TypeDependencyManagementHelper
  manageTypeDependencies(model::Binary &Binary) const {
    return TypeDependencyManagementHelper(ABI, Binary);
  }

public:
  /// Helper used for converting stack argument struct into
  /// the c-style representation
  ///
  /// \param StackArgumentTypes The qualified type of the relevant part of
  ///        the stack.
  /// \param IndexOffset The index of the first argument (should be set to
  ///        the number of register arguments).
  ///
  /// \return An ordered list of arguments.
  std::optional<llvm::SmallVector<model::Argument, 8>>
  tryConvertingStackArguments(model::QualifiedType StackArgumentTypes,
                              size_t IndexOffset) const;

  /// Helper used for deciding how an arbitrary return type should be
  /// distributed across registers and the stack accordingly to the \ref ABI.
  /// ABI.
  ///
  /// \param ReturnValueType The model type that should be returned.
  /// \return Information about registers and stack that are to be used to
  ///         return the said type.
  DistributedArgument
  distributeReturnValue(const model::QualifiedType &ReturnValueType) const;

  /// Helper used for deciding how an arbitrary set of arguments should be
  /// distributed across registers and the stack accordingly to the \ref ABI.
  ///
  /// \param Arguments The list of arguments to distribute.
  /// \param PassesReturnValueLocationAsAnArgument `true` if the first argument
  ///        slot should be occupied by a shadow return value, `false` otherwise
  /// \return Information about registers and stack that are to be used to
  ///         pass said arguments.
  DistributedArguments
  distributeArguments(const ArgumentVector &Arguments,
                      bool PassesReturnValueLocationAsAnArgument) const;

private:
  Converter::DistributedArguments
  Converter::distributePositionBasedArguments(const ArgumentVector &Arguments,
                                              std::size_t SkippedCount);

public:
  uint64_t finalStackOffset(const DistributedArguments &Arguments) const;

private:
  /// Checks that \param Function` is well-formed and is compatible with the
  /// `ABI`.
  ///
  /// \note: even if this does not guarantee full compatibility (since
  /// the conversion can still fail even if this passes), it is able to catch
  /// some obvious problems really early, which is often helpful.
  bool checkCompatibility(const model::RawFunctionType &Function) const;
};

using TDManager = Converter::TypeDependencyManagementHelper;

std::optional<model::TypePath>
Converter::toCABI(const model::RawFunctionType &Function,
                  TupleTree<model::Binary> &Binary) const {
  revng_log(Log, "Converting a `RawFunctionType` to `CABIFunctionType`.");
  revng_log(Log, "Function:\n" << serializeToString(Function));

  if (!checkCompatibility(Function)) {
    revng_log(Log,
              "FAIL: the function is not compatible with `"
                << model::ABI::getName(ABI.ABI) << "`.");
    return std::nullopt;
  }

  // A single place to conveniently store all the types that need to be added
  // to the model in order for the new function prototype to be valid.
  auto M = manageTypeDependencies(*Binary);

  // Convert register arguments first.
  auto RegisterArguments = M.tryConvertingRegisterArguments(Function.Arguments);
  if (RegisterArguments == std::nullopt) {
    revng_log(Log, "FAIL: Register argument conversion is not possible.");
    M.drop();
    return std::nullopt;
  }

  // Then stack arguments.
  auto Stack = tryConvertingStackArguments(Function.StackArgumentsType,
                                           RegisterArguments->size());
  if (Stack == std::nullopt) {
    revng_log(Log, "FAIL: Stack argument conversion is not possible.");
    M.drop();
    return std::nullopt;
  }

  // And the return type.
  auto ReturnType = M.tryConvertingReturnValue(Function.ReturnValues);
  if (ReturnType == std::nullopt) {
    revng_log(Log, "FAIL: Return value conversion is not possible.");
    M.drop();
    return std::nullopt;
  }

  // If all the partial conversions passed, the conversion is guaranteed to be
  // successful. All the `TypeDependencies` can now be recorded into the
  // model.
  M.commit();

  // With all the dependencies out of the way, new `CABIFunctionType` can now
  // be created,
  auto [NewType, NewTypePath] = Binary->newType<model::CABIFunctionType>();
  NewType.ID = Function.ID;
  NewType.CustomName = Function.CustomName;
  NewType.OriginalName = Function.OriginalName;
  NewType.ABI = ABI.ABI;

  // And filled in with the argument information.
  using model::Argument;
  for (auto &Argument : llvm::concat<Argument>(*RegisterArguments, *Stack))
    NewType.Arguments.insert(std::move(Argument));
  NewType.ReturnType = *ReturnType;

  revng_log(Log, "Conversion successful:\n" << serializeToString(NewType));

  // To finish up the conversion, remove all the references to the old type by
  // carefully replacing them with references to the new one.
  return replaceAllUsesWith(Function.key(), NewTypePath, Binary);
}

model::TypePath Converter::toRaw(const model::CABIFunctionType &Function,
                                 TupleTree<model::Binary> &Binary) const {
  revng_log(Log, "Converting a `CABIFunctionType` to `RawFunctionType`.");
  revng_log(Log, "Function:\n" << serializeToString(Function));

  const auto Architecture = model::ABI::getArchitecture(ABI.ABI);
  const auto RegisterSize = ModelArch::getPointerSize(Architecture);
  auto PointerQualifier = model::Qualifier::createPointer(RegisterSize);

  // Since this conversion cannot fail, nothing prevents us from creating
  // the result type right away.
  auto [NewType, NewTypePath] = Binary->newType<model::RawFunctionType>();
  NewType.CustomName = Function.CustomName;
  NewType.OriginalName = Function.OriginalName;
  NewType.ID = Function.ID;

  // Since shadow arguments are a concern, we need to deal with the return
  // value first.
  auto ReturnValue = distributeReturnValue(Function.ReturnType);
  if (!ReturnValue.Registers.empty()) {
    // The return value uses registers: pass them through to the new type.
    for (model::Register::Values Register : ReturnValue.Registers) {
      model::TypedRegister ReturnValueRegister;
      ReturnValueRegister.Location = Register;

      if (ReturnValue.Registers.size() > 1) {
        // TODO: We can do a better job of preserving types here, for example,
        //       if `Function.ReturnType` is a struct, we can try to use field
        //       types instead of just overriding everything with `Generic`.
        ReturnValueRegister.Type = genericRegisterType(Register, *Binary);
        revng_log(Log,
                  "Working on a multi-register return type: "
                    << serializeToString(Function.ReturnType) << ".");
      } else {
        auto M = manageTypeDependencies(*Binary);
        ReturnValueRegister.Type = M.chooseType(Function.ReturnType, Register);
      }

      NewType.ReturnValues.insert(std::move(ReturnValueRegister));
    }

    // If function returns a struct, make an effort to preserve the field
    // types.
    if (Function.ReturnType.Qualifiers.empty()) {
      const model::Type *Type = Function.ReturnType.UnqualifiedType.get();
      revng_assert(Type != nullptr);
      const auto *Struct = llvm::dyn_cast<model::StructType>(Type);
      if (Struct && Struct->Fields.size() == NewType.ReturnValues.size()) {
        revng_log(Log,
                  "Return value is a struct, making an attempt to preserve "
                  "field types.");

        using ModelRegister = model::Register::Values;
        SmallMap<ModelRegister, model::QualifiedType, 4> RecoveredTypes;
        size_t StructOffset = 0;
        for (size_t Index = 0; Index < Struct->Fields.size(); ++Index) {
          if (Index >= ABI.GeneralPurposeReturnValueRegisters.size()) {
            revng_log(Log,
                      "There are more fields ("
                        << Struct->Fields.size() << ") than registers ("
                        << ABI.GeneralPurposeReturnValueRegisters.size()
                        << "): aborting the preservation attempt.");
            break;
          }

          auto Register = ABI.GeneralPurposeReturnValueRegisters[Index];
          auto TypedRegisterIterator = NewType.ReturnValues.find(Register);
          if (TypedRegisterIterator == NewType.ReturnValues.end()) {
            revng_log(Log,
                      "An expected register ("
                        << model::Register::getName(Register)
                        << ") is not used: discarding its type:\n"
                        << serializeToString(Struct->Fields.at(Index)));
            continue;
          }

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

        // Only preserve types if there's one for each of the registers.
        // The worst thing we could do is to inject incompatible information
        // in.
        if (RecoveredTypes.size() == NewType.ReturnValues.size()) {
          for (auto [Register, Type] : RecoveredTypes)
            NewType.ReturnValues.at(Register).Type = Type;

          revng_log(Log, "The preservation attempt was successful.");
        } else {
          revng_log(Log, "The preservation attempt was not successful.");
        }
      }
    }
  } else if (ReturnValue.Size != 0) {
    // The return value uses a pointer-to-a-location: add it as an argument.

    auto MaybeReturnValueSize = Function.ReturnType.size();
    revng_assert(MaybeReturnValueSize != std::nullopt);
    revng_assert(ReturnValue.Size == *MaybeReturnValueSize);

    model::QualifiedType ReturnType = Function.ReturnType;
    ReturnType.Qualifiers.emplace_back(PointerQualifier);

    revng_assert(!ABI.GeneralPurposeReturnValueRegisters.empty());
    auto FirstRegister = ABI.GeneralPurposeReturnValueRegisters[0];
    model::TypedRegister ReturnPointer(FirstRegister);
    ReturnPointer.Type = std::move(ReturnType);
    NewType.ReturnValues.insert(std::move(ReturnPointer));
  } else {
    // The function returns `void`: no need to do anything special.
  }

  // Now that return value is figured out, the arguments are next.
  auto Arguments = distributeArguments(Function.Arguments,
                                       ReturnValue.SizeOnStack != 0);

  model::StructType StackArguments;
  uint64_t CombinedStackArgumentSize = 0;
  for (size_t ArgIndex = 0; ArgIndex < Arguments.size(); ++ArgIndex) {
    auto &ArgumentStorage = Arguments[ArgIndex];
    const auto &ArgumentType = Function.Arguments.at(ArgIndex).Type;

    // Transfer the register arguments first.
    if (!ArgumentStorage.Registers.empty()) {
      auto ArgumentName = Function.Arguments.at(ArgIndex).name();
      for (auto Register : ArgumentStorage.Registers) {
        model::NamedTypedRegister Argument(Register);

        if (ArgumentStorage.Registers.size() > 1) {
          // TODO: We can do a better job of preserving types here, for example,
          //       if this argument is a struct, we can try to use field
          //       types instead of just overriding everything with `Generic`.
          Argument.Type = genericRegisterType(Register, *Binary);
          revng_log(Log,
                    "Working on a multi-register return type: "
                      << serializeToString(Function.ReturnType) << ".");
        } else {
          auto M = manageTypeDependencies(*Binary);
          Argument.Type = M.chooseType(Function.ReturnType, Register);
        }

        // TODO: see what can be done to preserve names better
        if (llvm::StringRef{ ArgumentName.str() }.take_front(8) != "unnamed_")
          Argument.CustomName = ArgumentName;

        NewType.Arguments.insert(Argument);
      }
    }

    // Then stack arguments.
    if (ArgumentStorage.SizeOnStack != 0) {
      auto ArgumentIterator = Function.Arguments.find(ArgIndex);
      revng_assert(ArgumentIterator != Function.Arguments.end());
      const model::Argument &Argument = *ArgumentIterator;

      // Round the next offset based on the natural alignment.
      auto Alignment = abi::naturalAlignment(Argument.Type, ABI);
      revng_assert(Alignment.has_value() && Alignment.value() != 0);
      CombinedStackArgumentSize += (*Alignment
                                    - CombinedStackArgumentSize % *Alignment);

      // Each argument gets converted into a struct field.
      model::StructField Field;
      Field.Offset = CombinedStackArgumentSize;
      Field.CustomName = Argument.CustomName;
      Field.OriginalName = Argument.OriginalName;
      Field.Type = Argument.Type;
      StackArguments.Fields.insert(std::move(Field));

      // Compute the full size of the argument (including padding if needed),
      // so that the next argument is not placed into this occupied space.
      auto MaybeSize = Argument.Type.size();
      revng_assert(MaybeSize.has_value() && MaybeSize.value() != 0);
      CombinedStackArgumentSize += paddedSizeOnStack(MaybeSize.value(),
                                                     RegisterSize);
    }
  }

  // If the stack argument struct is not empty, record it into the model.
  if (CombinedStackArgumentSize != 0) {
    StackArguments.Size = CombinedStackArgumentSize;

    using namespace model;
    auto Type = UpcastableType::make<StructType>(std::move(StackArguments));
    NewType.StackArgumentsType = { Binary->recordNewType(std::move(Type)), {} };
  }

  // Set the final stack offset
  NewType.FinalStackOffset = finalStackOffset(Arguments);

  // Populate the list of preserved registers
  for (model::Register::Values Register : ABI.CalleeSavedRegisters)
    NewType.PreservedRegisters.insert(Register);

  revng_log(Log, "Conversion done:\n" << serializeToString(NewType));

  // To finish up the conversion, remove all the references to the old type by
  // carefully replacing them with references to the new one.
  return replaceAllUsesWith(Function.key(), NewTypePath, Binary);
}

template<typename RT>
std::optional<llvm::SmallVector<model::Argument, 8>>
TDManager::tryConvertingRegisterArguments(const SortedVector<RT> &Used) {
  llvm::SmallVector<model::Register::Values, 8> ArgumentRegisters;

  // Rely onto the register state deduction to make sure no "holes" are
  // present in-between the argument registers.
  abi::RegisterState::Map Map(model::ABI::getArchitecture(ABI.ABI));
  for (const RT &Register : Used)
    Map[Register.Location] = abi::RegisterState::Yes;
  auto Enforced = abi::enforceRegisterStateDeductions(Map, ABI);
  for (auto [Register, Pair] : Enforced)
    if (abi::RegisterState::shouldEmit(Pair.IsUsedForPassingArguments))
      ArgumentRegisters.emplace_back(Register);

  // But just knowing which registers we need is not sufficient, we also have
  // to order them properly.
  auto Ordered = abi::orderArguments(ArgumentRegisters, ABI);

  llvm::SmallVector<model::Argument, 8> Result;
  for (model::Register::Values Register : Ordered) {
    auto CurrentRegisterIterator = Used.find(Register);
    if (CurrentRegisterIterator != Used.end()) {
      // If the current register is confirmed to be in use, convert it into
      // an argument while preserving its type and name.
      auto &Current = Result.emplace_back();
      if (Used.at(Register).Type.get() != nullptr)
        Current.Type = Used.at(Register).Type;
      else
        Current.Type = defaultRegisterType(Register, Binary);
      Current.CustomName = Used.at(Register).CustomName;
    } else {
      // This register is unused but we still have to create an argument
      // for it. Otherwise the resulting function will not be compatible with
      // the current one: if the "hole" is not preserved, there will be no way
      // to recreate it at any point in the future, when, for example,
      // converting back to the original-like representation.
      auto &Current = Result.emplace_back();
      if (Used.at(Register).Type.get() != nullptr)
        Current.Type = Used.at(Register).Type;
      else
        Current.Type = defaultRegisterType(Register, Binary);
      Current.CustomName = Used.at(Register).CustomName;
    }
  }

  // Rewrite all the indices to make sure they are consistent.
  for (auto Pair : llvm::enumerate(llvm::reverse(Result)))
    Pair.value().Index = Pair.index();

  return Result;
}

std::optional<llvm::SmallVector<model::Argument, 8>>
Converter::tryConvertingStackArguments(model::QualifiedType StackArgumentTypes,
                                       size_t IndexOffset) const {
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

  // If the struct is empty, it indicates that there are no stack arguments.
  if (Types.Fields.empty()) {
    revng_assert(Types.size() == 0);
    return llvm::SmallVector<model::Argument, 8>{};
  }

  // Compute the alignment of the first argument.
  auto CurrentAlignment = *abi::naturalAlignment(Types.Fields.begin()->Type,
                                                 ABI);
  if (!IsZeroOrPowerOfTwo(CurrentAlignment)) {
    revng_log(Log,
              "The natural alignment of a type is not a power of two:\n"
                << serializeToString(Types.Fields.begin()->Type));
    return std::nullopt;
  }

  // Process each of the fields (except for the very last one),
  // converting them into arguments separately.
  llvm::SmallVector<model::Argument, 8> Result;
  for (auto Iterator = Types.Fields.begin();
       Iterator != std::prev(Types.Fields.end());
       ++Iterator) {
    const auto &CurrentArgument = *Iterator;
    auto MaybeSize = CurrentArgument.Type.size();
    revng_assert(MaybeSize.has_value() && MaybeSize.value() != 0);

    const auto Architecture = model::ABI::getArchitecture(ABI.ABI);
    const auto RegisterSize = ModelArch::getPointerSize(Architecture);
    uint64_t CurrentSize = paddedSizeOnStack(MaybeSize.value(), RegisterSize);

    const auto &NextArgument = *std::next(Iterator);
    auto NextAlignment = *abi::naturalAlignment(NextArgument.Type, ABI);
    if (!IsZeroOrPowerOfTwo(NextAlignment)) {
      revng_log(Log,
                "The natural alignment of a type is not a power of two:\n"
                  << serializeToString(NextArgument.Type));
      return std::nullopt;
    }

    if (CurrentArgument.Offset + CurrentSize != NextArgument.Offset
        || (CurrentArgument.Offset + CurrentSize) % NextAlignment != 0) {
      revng_log(Log,
                "The natural alignment of a type would make it impossible "
                "to represent as CABI, abandon the conversion.\nTODO: we "
                "might want to preprocess such functions and manually "
                "\"fill\" the holes in before attempting the conversion.");
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

  // And don't forget the very last field.
  const model::StructField &LastArgument = *std::prev(Types.Fields.end());
  model::Argument &New = Result.emplace_back();
  New.Index = IndexOffset++;
  New.Type = LastArgument.Type;
  New.CustomName = LastArgument.CustomName;
  New.OriginalName = LastArgument.OriginalName;

  // Make sure the size does not contradict the final alignment.
  auto FullAlignment = *abi::naturalAlignment(StackArgumentTypes, ABI);
  if (!IsZeroOrPowerOfTwo(FullAlignment)) {
    revng_log(Log,
              "The natural alignment of a type is not a power of two:\n"
                << serializeToString(Types));
    return std::nullopt;
  }
  revng_assert(Types.Size % FullAlignment == 0);

  return Result;
}

using ModelRegister = model::Register::Values;
template<typename RegisterType>
std::optional<model::QualifiedType>
TDManager::tryConvertingReturnValue(const SortedVector<RegisterType> &Used) {
  if (Used.size() == 0) {
    // The function doesn't return anything.
    return Binary.getPrimitiveType(model::PrimitiveTypeKind::Void, 0);
  }

  const auto Architecture = model::ABI::getArchitecture(ABI.ABI);

  // The case where the only used register is the pointer-to-copy location
  // needs to be handled in a special way.
  if (Used.size() == 1
      && Used.begin()->Location == ABI.ReturnValueLocationRegister) {
    if (Used.begin()->Type.isPointer()) {
      // If the type is already a pointer, preserve it.
      return Used.begin()->Type;
    } else if (!Used.begin()->Type.isPrimitive()) {
      // If type is a primitive, rewrite it with an arbitrary pointer type.
      auto Byte = Binary.getPrimitiveType(model::PrimitiveTypeKind::Generic, 8);
      return model::QualifiedType(Byte, {}).getPointerTo(Architecture);
    } else {
      revng_log(Log,
                "Abort the conversion since proceeding would mean discarding "
                "an arbitrary non-scalar type.");
      return std::nullopt;
    }
  }

  // The return value is register-based, deal with it similarly to how the
  // arguments are worked with, which requires relying on register state
  // deduction and ordering.
  abi::RegisterState::Map Map(model::ABI::getArchitecture(ABI.ABI));
  for (const RegisterType &Register : Used)
    Map[Register.Location] = abi::RegisterState::Yes;
  auto Enforced = abi::enforceRegisterStateDeductions(Map, ABI);

  llvm::SmallSet<model::Register::Values, 8> ArgumentRegisters;
  for (auto [Register, Pair] : Enforced)
    if (abi::RegisterState::shouldEmit(Pair.IsUsedForPassingArguments))
      ArgumentRegisters.insert(Register);
  auto Ordered = abi::orderArguments(ArgumentRegisters, ABI);

  if (Ordered.size() == 1) {
    if (auto Iterator = Used.find(*Ordered.begin()); Iterator != Used.end()) {
      // Only one register is used, just return its type.
      return Iterator->Type;
    } else {
      // One register is used but its type cannot be obtained.
      // Create a register sized return type instead.
      return defaultRegisterType(*Ordered.begin(), Binary);
    }
  } else {
    // Multiple registers mean that it should probably be a struct.
    //
    // \note: it's also possible that it's just a big primitive,
    // \todo: look into supporting those.
    using ST = model::StructType;
    auto Ptr = Dependencies.emplace_back(model::UpcastableType::make<ST>());
    ST &ReturnedStruct = *llvm::dyn_cast<ST>(Ptr.get());
    for (auto Register : Ordered) {
      // Make a separate field for each register.
      model::StructField Field;
      Field.Offset = ReturnedStruct.Size;
      if (auto Iterator = Used.find(*Ordered.begin()); Iterator != Used.end())
        Field.Type = Iterator->Type;
      else
        Field.Type = genericRegisterType(*Ordered.begin(), Binary);

      auto FieldSize = Field.Type.size();
      revng_assert(FieldSize.has_value() && FieldSize.value() != 0);

      // Round the next offset based on the natural alignment.
      auto Alignment = abi::naturalAlignment(Field.Type, ABI);
      revng_assert(Alignment.has_value() && Alignment.value() != 0);
      ReturnedStruct.Size += (*Alignment - ReturnedStruct.Size % *Alignment);

      // Insert the field
      ReturnedStruct.Fields.insert(std::move(Field));

      // Update the total struct size: insert some padding if necessary.
      auto RegisterSize = model::Architecture::getPointerSize(Architecture);
      ReturnedStruct.Size += paddedSizeOnStack(FieldSize.value(), RegisterSize);
    }

    revng_assert(ReturnedStruct.Size != 0 && !ReturnedStruct.Fields.empty());
    return Binary.getTypePath(ReturnedStruct.key());
  }
}

Converter::DistributedArguments
Converter::distributePositionBasedArguments(const ArgumentVector &Arguments,
                                            std::size_t SkippedCount) {
  DistributedArguments Result;

  for (const model::Argument &Argument : Arguments) {
    std::size_t RegisterIndex = Argument.Index + SkippedCount;
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

Converter::DistributedArguments
Converter::distributeNonPositionBasedArguments(const ArgumentVector &Arguments,
                                               std::size_t SkippedCount) const {
  DistributedArguments Result;
  size_t UsedGeneralPurposeRegisterCounter = SkippedCount;
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

Converter::DistributedArguments
Converter::distributeArguments(const ArgumentVector &Arguments,
                               bool PassesReturnValueLocationAsAnArgument) {
  bool SkippedRegisterCount = 0;

  if (PassesReturnValueLocationAsAnArgument == true) {
    revng_assert(verifyReturnValueLocation(ABI),
                 "ABIs where non-first argument GPR is used for passing the "
                 "address of the space allocated for big return values are "
                 "not currently supported.");

    const auto &GPRs = ABI.GeneralPurposeArgumentRegisters;
    if (!GPRs.empty() && ABI.ReturnValueLocationRegister == GPRs[0])
      SkippedRegisterCount = 1;
  }

  if (ABI.ArgumentsArePositionBased)
    return distributePositionBasedArguments(Arguments, SkippedRegisterCount);
  else
    return distributeNonPositionBasedArguments(Arguments, SkippedRegisterCount);
}

Converter::DistributedArgument
Converter::distributeReturnValue(const model::QualifiedType &ReturnValueType) {
  if (ReturnValueType.isVoid())
    return DistributedArgument{};

  auto MaybeSize = ReturnValueType.size();
  revng_assert(MaybeSize.has_value());

  if (ReturnValueType.isFloat()) {
    const auto &Registers = ABI.VectorReturnValueRegisters;
    // TODO: replace `UnlimitedRegisters` with the actual value to be defined
    //       by the trait.
    const size_t L = UnlimitedRegisters;
    return considerRegisters(*MaybeSize, L, 0, Registers, false).first;
  } else {
    const size_t L = ReturnValueType.isScalar() ?
                       ABI.MaximumGPRsPerScalarReturnValue :
                       ABI.MaximumGPRsPerAggregateReturnValue;
    const auto &Registers = ABI.GeneralPurposeReturnValueRegisters;
    return considerRegisters(*MaybeSize, L, 0, Registers, false).first;
  }
}

model::QualifiedType
TDManager::chooseType(const model::QualifiedType &ArgumentType,
                      model::Register::Values Register) {
  auto MaybeSize = ArgumentType.size();
  auto TargetSize = model::Register::getSize(Register);

  if (!MaybeSize.has_value()) {
    return defaultRegisterType(Register, Binary);
  } else if (*MaybeSize > TargetSize) {
    // TODO: additional considerations should be made here
    //       for better position-based ABI support.
    const auto Architecture = model::ABI::getArchitecture(ABI.ABI);
    return ArgumentType.getPointerTo(Architecture);
  } else if (!ArgumentType.isScalar()) {
    // TODO: this can be considerably improved, we can preserve more
    //       information here over just labeling it as `Generic`.
    return genericRegisterType(Register, Binary);
  }

  return ArgumentType;
}

/// Takes care of calculation the final stack offset.
uint64_t Converter::finalStackOffset(const DistributedArguments &Arguments) {
  constexpr auto Architecture = model::ABI::getArchitecture(ABI.ABI);
  uint64_t Result = model::Architecture::getCallPushSize(Architecture);
  if (ABI.CalleeIsResponsibleForStackCleanup) {
    for (auto &Argument : Arguments)
      Result += Argument.SizeOnStack;

    // TODO: take return values into the account.

    // TODO: take shadow space into the account if relevant.

    revng_assert(IsZeroOrPowerOfTwo(ABI.StackAlignment));
    Result += ABI.StackAlignment - 1;
    Result &= ~(ABI.StackAlignment - 1);
  }

  return Result;
}

// clang-format off
bool
Converter::checkCompatibility(const model::RawFunctionType &Function) const {
  // clang-format on
  return skippingEnumSwitch<1>(Arch, [&]<model::Architecture::Values A>() {
    // Check argument registers.
    if (!verify<A>(Function.Arguments, D.GeneralPurposeArgumentRegisters))
      return std::nullopt;

    // Check return value registers.
    if (!verify<A>(Function.ReturnValues, D.GeneralPurposeReturnValueRegisters))
      return std::nullopt;

    // If return value location register is provided, check its architecture.
    model::Register::Values ReturnValueLocation = D.ReturnValueLocationRegister;
    if (ReturnValueLocation != model::Register::Invalid)
      revng_assert(model::Register::getArchitecture(ReturnValueLocation) == A);

    // Check the architecture of callee saved registers.
    for (auto &SavedRegister : D.CalleeSavedRegisters)
      revng_assert(model::Register::getArchitecture(SavedRegister) == A);
  });
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

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
      Result.ReturnValues.emplace_back().Registers = std::move(RV.Registers);
      Result.ReturnValues.back().Type = Function.ReturnType;
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
    ReturnValues.emplace_back().Registers = { Register.Location };
    ReturnValues.back().Type = Register.Type;
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
  for (model::Register::Values Register : argumentRegisters())
    if (!VerificationHelper(Register))
      return false;

  // Verify return values
  LookupHelper.clear();
  for (model::Register::Values Register : returnValueRegisters())
    if (!VerificationHelper(Register))
      return false;

  return true;
}

size_t Layout::argumentRegisterCount() const {
  size_t Result = 0;

  for (const Argument &Argument : Arguments)
    Result += Argument.Registers.size();

  return Result;
}

size_t Layout::returnValueRegisterCount() const {
  size_t Result = 0;

  for (const ReturnValue &ReturnValue : ReturnValues)
    Result += ReturnValue.Registers.size();

  return Result;
}

llvm::SmallVector<model::Register::Values, 8>
Layout::argumentRegisters() const {
  llvm::SmallVector<model::Register::Values, 8> Result;

  for (const Argument &Argument : Arguments)
    Result.append(Argument.Registers.begin(), Argument.Registers.end());

  return Result;
}

llvm::SmallVector<model::Register::Values, 8>
Layout::returnValueRegisters() const {
  llvm::SmallVector<model::Register::Values, 8> Result;

  for (const ReturnValue &ReturnValue : ReturnValues)
    Result.append(ReturnValue.Registers.begin(), ReturnValue.Registers.end());

  return Result;
}

} // namespace abi::FunctionType
