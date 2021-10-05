/// \file ABI.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/DenseMap.h"

#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/VerifyHelper.h"
#include "revng/StackAnalysis/ABI.h"

namespace abi {

using namespace model::abi;

template<size_t Size>
using RegisterArray = std::array<model::Register::Values, Size>;

template<model::Architecture::Values Architecture, size_t AllowedRegisterCount>
static bool
verify(const RegisterArray<AllowedRegisterCount> &AllowedRegisters) {
  for (const auto &Register : AllowedRegisters) {
    // Check if the registers of the same architecture are used.
    if (model::Register::getArchitecture(Register) != Architecture)
      return false;

    // Check if there are any duplicates in allowed register container.
    if (llvm::count(AllowedRegisters, Register) != 1)
      return false;
  }

  return true;
}

template<model::Architecture::Values Architecture, typename RegisterType>
static bool verify(const SortedVector<RegisterType> &UsedRegisters) {
  for (const auto &Register : UsedRegisters) {
    // Check if the registers of the same architecture are used.
    if (model::Register::getArchitecture(Register) != Architecture)
      return false;
  }

  return true;
}

template<typename RegisterType,
         size_t GenericRegisterCount,
         size_t VectorRegisterCount>
static std::optional<llvm::SmallVector<RegisterType, GenericRegisterCount>>
replImpl(const SortedVector<RegisterType> &UsedRegisters,
         const RegisterArray<GenericRegisterCount> &AllowedGenericRegisters,
         const RegisterArray<VectorRegisterCount> &AllowedVectorRegisters) {
  llvm::SmallVector<RegisterType, GenericRegisterCount> Result;

  size_t UsedCount = 0;
  bool MustBeUsed = false;
  size_t Index = AllowedGenericRegisters.size();
  for (; Index != size_t(-1); --Index) {
    bool IsUsed = false;
    auto GenericIterator = UsedRegisters.find(AllowedGenericRegisters[Index]);
    if (GenericIterator != UsedRegisters.end()) {
      Result.emplace_back(*GenericIterator);
      IsUsed = true;
    } else if (!AllowedVectorRegisters.empty()) {
      auto VectorIterator = UsedRegisters.find(AllowedVectorRegisters[Index]);
      if (VectorIterator != UsedRegisters.end()) {
        Result.emplace_back(*VectorIterator);
        IsUsed = true;
      }
    }

    if (!IsUsed && MustBeUsed)
      return std::nullopt;

    MustBeUsed = MustBeUsed || IsUsed;
  }

  return Result;
}

template<typename RegisterType,
         size_t GenericRegisterCount,
         size_t VectorRegisterCount>
static std::optional<llvm::SmallVector<RegisterType, GenericRegisterCount>>
nreplImpl(const SortedVector<RegisterType> &UsedRegisters,
          const RegisterArray<GenericRegisterCount> &AllowedGenericRegisters,
          const RegisterArray<VectorRegisterCount> &AllowedVectorRegisters) {
  llvm::SmallVector<RegisterType, GenericRegisterCount> Result;

  auto SelectUsed = [&Result](const auto &RegisterContainer) -> bool {
    size_t UsedCount = 0;
    bool MustBeUsed = false;
    for (model::Register::Values Register : RegisterContainer) {
      bool IsUsed = UsedRegisters.count(Register) != 0;
      if (IsUsed)
        Result.emplace_back(Register);

      if (!IsUsed && MustBeUsed)
        return false;

      MustBeUsed = MustBeUsed || IsUsed;
    }

    return true;
  };

  if (!SelectUsed(llvm::reverse(AllowedVectorRegisters)))
    return std::nullopt;

  if (!SelectUsed(llvm::reverse(AllowedGenericRegisters)))
    return std::nullopt;

  return Result;
}

template<model::Architecture::Values Architecture,
         bool VectorArgumentsReplaceGenericOnes,
         typename RegisterType,
         size_t GenericRegisterCount,
         size_t VectorRegisterCount>
static std::optional<llvm::SmallVector<RegisterType, GenericRegisterCount>>
analyze(const SortedVector<RegisterType> &UsedRegisters,
        const RegisterArray<GenericRegisterCount> &AllowedGenericRegisters,
        const RegisterArray<VectorRegisterCount> &AllowedVectorRegisters) {
  revng_assert(verify<Architecture>(UsedRegisters));
  revng_assert(verify<Architecture>(AllowedRegisters));
  revng_assert(verify<Architecture>(FallbackRegisters));

  if constexpr (VectorArgumentsReplaceGenericOnes) {
    bool C = AllowedGenericRegisters.size() == AllowedVectorRegisters.size();
    revng_assert(C);
  }

  // Ensure all the used registers are allowed
  for (const auto &Register : UsedRegisters)
    if (llvm::count(AllowedGenericRegisters, Register.Location) != 1)
      if (llvm::count(AllowedVectorRegisters, Register.Location) != 1)
        return std::nullopt;

  // Ensure the register usage continuity, e. g. if the register for the second
  // parameter is used, the register for the first one must be used as well.
  if constexpr (VectorArgumentsReplaceGenericOnes)
    return replImpl(UsedRegisters,
                    AllowedGenericRegisters,
                    AllowedVectorRegisters);
  else
    return nreplImpl(UsedRegisters,
                     AllowedGenericRegisters,
                     AllowedVectorRegisters);
}

template<model::abi::Values V>
bool ABI<V>::isCompatible(const model::RawFunctionType &Explicit) {
  static constexpr auto Arch = getArchitecture(V);
  bool ArgumentAnalysis = analyze<Arch>(Explicit.Arguments,
                                        Allowed::GenericArgumentRegisters,
                                        Allowed::VectorArgumentRegisters);
  bool ReturnValueAnalysis = analyze<Arch>(Explicit.ReturnValues,
                                           Allowed::GenericReturnValueRegisters,
                                           Allowed::VectorReturnValueRegisters);
  return ArgumentAnalysis && ReturnValueAnalysis;
}

model::PrimitiveTypeKind::Values
selectTypeKind(model::Register::Type::Values RegisterType) {
  switch (RegisterType) {
  case model::Register::Type::Generic:
    return model::PrimitiveTypeKind::PointerOrNumber;

  case model::Register::Type::FloatingPoint:
    return model::PrimitiveTypeKind::Float;

  case Count:
  case Invalid:
  default:
    revng_abort();
  }
}

model::QualifiedType
buildType(model::Register::Values Register, model::Binary &TheBinary) {
  auto Kind = selectTypeKind(model::Register::getType(Register));
  auto Size = model::Register::getSize(Register);
  return model::QualifiedType{ TheBinary.getPrimitiveType(Kind, Size) };
}

template<model::abi::Values V>
std::optional<model::CABIFunctionType>
ABI<V>::toCABI(model::Binary &TheBinary,
               const model::RawFunctionType &Explicit) {
  static constexpr auto A = getArchitecture(V);
  auto AnalyzedArguments = analyze<A>(Explicit.Arguments,
                                      Allowed::GenericArgumentRegisters,
                                      Allowed::VectorArgumentRegisters);
  auto AnalyzedReturnValues = analyze<A>(Explicit.ReturnValues,
                                         Allowed::GenericReturnValueRegisters,
                                         Allowed::VectorReturnValueRegisters);

  if (!AnalizedArguments.has_value() || !AnalizedReturnValues.has_value())
    return {};

  model::CABIFunctionType Result;
  Result.ABI = SystemV_x86_64;

  //
  // Build return type
  //
  if (AnalyzedReturnValues->size() == 0) {
    Result.ReturnType = model::QualifiedType{
      TheBinary.getPrimitiveType(model::PrimitiveTypeKind::Void, 0)
    };
  } else if (AnalyzedReturnValues->size() == 1) {
    Result.ReturnType = buildType(AnalyzedReturnValues->front(), TheBinary);
  } else {
    size_t Offset = 0;

    auto NewType = makeType<StructType>();
    auto *MultipleReturnValues = llvm::cast<StructType>(NewType.get());
    for (const auto &Register : AnalyzedReturnValues.value()) {
      model::StructField NewField;
      NewField.Offset = Offset;
      NewField.Type = buildType(Register, TheBinary);
      MultipleReturnValues->Fields.insert(std::move(NewField));

      Offset += model::Register::getSize(Register);
    }
    MultipleReturnValues->Size = Offset;

    Result.ReturnType = { TheBinary.recordNewType(std::move(NewType)), {} };
  }

  //
  // Build argument list
  //
  for (size_t Index = 0; auto Register : ArgumentAnalysis.value()) {
    model::Argument NewArgument;
    NewArgument.Index = Index++;
    NewArgument.Type = buildType(Register, TheBinary);
    NewArgument.CustomName = Explicit.Arguments.at(Register).CustomName;
    Result.Arguments.insert(std::move(NewArgument));
  }

  return Result;
}

using IndexType = decltype(model::Argument::Index);
struct InternalArguments {
  llvm::DenseMap<IndexType, model::Register::Values> Register;
  SortedVector<IndexType> Stack;
};
template<typename Allowed>
static std::optional<InternalArguments>
allocateRegisters(const SortedVector<model::Argument> &Arguments) {
  InternalArguments Result;
  size_t UsedGenericRegisterCounter = 0;
  size_t UsedVectorRegisterCounter = 0;

  using OR = std::optional<model::Register::Values>;
  auto GetTheNextGenericRegister = [&UsedGenericRegisterCounter]() -> OR {
    if (UsedGenericRegisterCounter < Allowed::GenericArgumentRegisters.size())
      return Allowed::GenericArgumentRegisters[UsedGenericRegisterCounter];
    else
      return std::nullopt;
  };
  auto GetTheNextVectorRegister = [&UsedVectorRegisterCounter]() -> OR {
    if (UsedVectorRegisterCounter < Allowed::VectorArgumentRegisters.size())
      return Allowed::VectorArgumentRegisters[UsedVectorRegisterCounter];
    else
      return std::nullopt;
  };

  for (const model::Argument &Argument : Arguments) {
    if (!Argument.Type.isScalar()) // This could probably be an assert.
      return std::nullopt;

    if (!Argument.Type.isFloat()) {
      auto NextRegister = GetTheNextGenericRegister();
      if (!NextRegister.has_value()) {
        StackArguments.emplace(Argument.Index);
        continue;
      }
      auto NextSize = model::Register::getSize(*NextRegister);

      auto MaybeSize = Argument.Type.size(VH);
      revng_assert(MaybeSize);

      // TODO
      // if constexpr (Allowed::AllowAnArgumentToOccupySubsequentRegisters) {
      //
      // } else {
      if (*MaybeSize > NextSize) {
        StackArguments.emplace(Argument.Index);
      } else {
        RegisterArguments.try_emplace(Argument.Index, *NextRegister);
        ++UsedGenericRegisterCounter;
        if constexpr (Allowed::VectorArgumentsReplaceGenericOnes)
          UsedVectorRegisterCounter = UsedGenericRegisterCounter;
      }
      // }

    } else {
      auto NextRegister = GetTheNextVectorRegister();
      if (!NextRegister.has_value()) {
        StackArguments.emplace(Argument.Index);
        continue;
      }
      auto NextSize = model::Register::getSize(*NextRegister);

      auto MaybeSize = Argument.Type.size(VH);
      revng_assert(MaybeSize);

      if (*MaybeSize > NextSize) {
        StackArguments.emplace(Argument.Index);
      } else {
        RegisterArguments.try_emplace(Argument.Index, *NextRegister);
        ++UsedVectorRegisterCounter;
        if constexpr (Allowed::VectorArgumentsReplaceGenericOnes)
          UsedGenericRegisterCounter = UsedVectorRegisterCounter;
      }
    }
  }
}

template<model::abi::Values V>
std::optional<model::RawFunctionType>
ABI<V>::toRaw(model::Binary &TheBinary,
              const model::CABIFunctionType &Original) {
  auto Arguments = allocateRegisters<Allowed>(Original.Arguments);
  if (!Arguments.has_values())
    return std::nullopt;
  auto [RegisterArguments, StackArguments] = *Arguments;

  model::RawFunctionType Result;
  for (auto [ArgumentIndex, Register] : RegisterArguments) {
    model::NamedTypedRegister Argument(Register);
    Argument.Type = buildType(Register, TheBinary);
    Argument.CustomName = Original.Arguments.at(ArgumentIndex).CustomName;
    Result.Arguments.insert(Argument);
  }
  for (auto ArgumentIndex : StackArguments) {
    // TODO: handle stack arguments.
  }

  // Allocate return values
  if (!Original.ReturnType.isVoid()) {
    if (!Original.ReturnType.isScalar()) // This could probably be an assert.
      return std::nullopt;

    auto MaybeSize = Original.ReturnType.size(VH);
    revng_assert(MaybeSize.has_value());

    if (!Original.ReturnType.isFloat()) {
      // TODO
      // if constexpr (Allowed::AllowAnArgumentToOccupySubsequentRegisters) {
      //
      // } else {
      revng_assert(!Allowed::GenericReturnValueRegisters.empty());
      auto ReturnRegister = Allowed::GenericReturnValueRegisters[0];
      auto Size = model::Register::getSize(ReturnRegister);
      revng_assert(*MaybeSize <= Size);

      model::TypedRegister Argument(ReturnRegister);
      Argument.Type = buildType(ReturnRegister, TheBinary);
      Result.ReturnValues.insert(Argument);
      // }
    } else {
      // TODO
      // if constexpr (Allowed::AllowAnArgumentToOccupySubsequentRegisters) {
      //
      // } else {
      revng_assert(!Allowed::VectorReturnValueRegisters.empty());
      auto ReturnRegister = Allowed::VectorReturnValueRegisters[0];
      auto Size = model::Register::getSize(ReturnRegister);
      revng_assert(*MaybeSize <= Size);

      model::TypedRegister Argument(ReturnRegister);
      Argument.Type = buildType(ReturnRegister, TheBinary);
      Result.ReturnValues.insert(Argument);
      // }
    }
  }

  // Populate the list of preserved registers
  for (auto Register : Allowed::CalleeSavedRegisters)
    Result.PreservedRegisters.insert(Register);

  return Result;
}

template<model::abi::Values V>
model::TypePath ABI<V>::defaultPrototype(model::Binary &TheBinary) {
  auto NewType = model::makeType<model::RawFunctionType>();
  auto TypePath = TheBinary.recordNewType(std::move(NewType));
  auto &T = *llvm::cast<model::RawFunctionType>(TypePath.get());

  for (auto Register : Allowed::GenericArgumentRegisters) {
    model::NamedTypedRegister Argument(Register);
    Argument.Type = buildType(Register, TheBinary);
    T.Arguments.insert(Argument);
  }
  // for (auto Register : Allowed::VectorArgumentRegisters) {
  //   model::NamedTypedRegister Argument(Register);
  //   Argument.Type = buildType(Register, TheBinary);
  //   T.Arguments.insert(Argument);
  // }

  for (auto Register : Allowed::GenericReturnValueRegisters) {
    model::TypedRegister ReturnValue(Register);
    ReturnValue.Type = buildType(Register, TheBinary);
    T.ReturnValues.insert(ReturnValue);
  }
  // for (auto Register : Allowed::VectorReturnValueRegisters) {
  //   model::TypedRegister ReturnValue(Register);
  //   ReturnValue.Type = buildType(Register, TheBinary);
  //   T.ReturnValues.insert(ReturnValue);
  // }

  for (auto Register : CalleeSavedRegisters)
    T.PreservedRegisters.insert(Register);

  return TypePath;
}

// TODO: implement this!
void ABI<SystemV_x86_64>::applyDeductions(RegisterStateMap &Prototype) {
  static_cast<void>(Prototype);
  // using namespace model::RegisterState;

  // // Find the highest-indexed YesOrDead argument, and mark YesOrDead all
  // those
  // // before it. Same for return values.
  // bool ArgumentMatch = false;
  // for (auto Register : llvm::reverse(Allowed::GenericArgumentRegisters)) {
  //   auto State = getOrDefault(Prototype,
  //                             Register,
  //                             { model::RegisterState::Invalid,
  //                               model::RegisterState::Invalid });

  //   auto AsArgument = State.first;

  //   if (not ArgumentMatch) {
  //     ArgumentMatch = isYesOrDead(AsArgument);
  //   } else if (AsArgument != Yes and AsArgument != Dead) {
  //     Prototype[Register].first = YesOrDead;
  //   }
  // }

  // bool ReturnValueMatch = false;
  // for (auto Register : llvm::reverse(Allowed::GenericReturnValueRegisters)) {
  //   auto State = getOrDefault(Prototype,
  //                             Register,
  //                             { model::RegisterState::Invalid,
  //                               model::RegisterState::Invalid });

  //   auto AsReturnValue = State.second;

  //   if (not ReturnValueMatch) {
  //     ReturnValueMatch = isYesOrDead(AsReturnValue);
  //   } else if (AsReturnValue != Yes and AsReturnValue != Dead) {
  //     Prototype[Register].second = YesOrDead;
  //   }
  // }

  // // Mark all the other non-YesOrDead as No
  // for (auto &[Register, State] : Prototype) {
  //   auto &[AsArgument, AsReturnValue] = State;

  //   if (not isYesOrDead(AsArgument))
  //     AsArgument = No;

  //   if (not isYesOrDead(AsReturnValue))
  //     AsReturnValue = No;
  // }
}

//
// Specializations
//

template class ABI<model::abi::SystemV_x86_64>;
template class ABI<model::abi::Microsoft_x64>;
// More specializations are to come

} // namespace abi
