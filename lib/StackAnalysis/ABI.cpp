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

template<model::Architecture::Values Architecture,
         typename RegisterType,
         size_t RegisterCount>
bool verify(const SortedVector<RegisterType> &UsedRegisters,
            const RegisterArray<RegisterCount> &AllowedRegisters) {
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
      return false;
  }

  // Verify that every used register is also allowed.
  for (const RegisterType &Register : UsedRegisters)
    if (llvm::count(AllowedRegisters, Register.Location) != 1)
      return false;

  return true;
}

constexpr static model::PrimitiveTypeKind::Values
selectTypeKind(model::Register::Values) {
  // TODO: implement some kind of way to determine the register size.
  // At the very least we should be able to differentiate general purpose
  // registers from vector ones.

  return model::PrimitiveTypeKind::PointerOrNumber;
}

static model::QualifiedType getType(model::PrimitiveTypeKind::Values Kind,
                                    size_t Size,
                                    model::Binary &TheBinary) {
  return model::QualifiedType{ TheBinary.getPrimitiveType(Kind, Size) };
}

static model::QualifiedType
buildType(model::Register::Values Register, model::Binary &TheBinary) {
  model::PrimitiveTypeKind::Values Kind = selectTypeKind(Register);
  size_t Size = model::Register::getSize(Register);
  return getType(Kind, Size, TheBinary);
}

// static std::optional<model::QualifiedType>
// buildDoubleType(model::Register::Values UpperRegister,
//                 model::Register::Values LowerRegister,
//                 model::Binary &TheBinary) {
//   model::PrimitiveTypeKind::Values UpperKind = selectTypeKind(UpperRegister);
//   model::PrimitiveTypeKind::Values LowerKind = selectTypeKind(LowerRegister);
//   if (UpperKind != LowerKind)
//     return std::nullopt;
//
//   size_t UpperSize = model::Register::getSize(UpperRegister);
//   size_t LowerSize = model::Register::getSize(LowerRegister);
//   return getType(UpperKind, UpperSize + LowerSize, TheBinary);
// }

static std::optional<model::QualifiedType>
buildDoubleType(model::Register::Values UpperRegister,
                model::Register::Values LowerRegister,
                model::PrimitiveTypeKind::Values CustomKind,
                model::Binary &TheBinary) {
  model::PrimitiveTypeKind::Values UpperKind = selectTypeKind(UpperRegister);
  model::PrimitiveTypeKind::Values LowerKind = selectTypeKind(LowerRegister);
  if (UpperKind != LowerKind)
    return std::nullopt;

  size_t UpperSize = model::Register::getSize(UpperRegister);
  size_t LowerSize = model::Register::getSize(LowerRegister);
  return getType(CustomKind, UpperSize + LowerSize, TheBinary);
}

//
// toCABI
//

template<typename CC, typename RegisterType, size_t RegisterCount>
static std::optional<llvm::SmallVector<model::Argument, 8>>
convertArguments(const SortedVector<RegisterType> &UsedRegisters,
                 const RegisterArray<RegisterCount> &AllowedRegisters,
                 model::Binary &TheBinary) {
  llvm::SmallVector<model::Argument, 8> Result;

  bool MustUseTheNextOne = false;
  auto AllowedRegisterRange = llvm::enumerate(llvm::reverse(AllowedRegisters));
  for (auto Pair : AllowedRegisterRange) {
    size_t Index = AllowedRegisters.size() - Pair.index() - 1;
    model::Register::Values Register = Pair.value();
    bool IsUsed = UsedRegisters.find(Register) != UsedRegisters.end();
    if (IsUsed) {
      model::Argument Temporary;
      Temporary.Type = buildType(Register, TheBinary);
      Temporary.CustomName = UsedRegisters.at(Register).CustomName;
      Result.emplace_back(Temporary);
    } else if (MustUseTheNextOne) {
      if constexpr (!CC::OnlyStartDoubleArgumentsFromAnEvenRegister) {
        return std::nullopt;
      } else if ((Index & 1) == 0) {
        return std::nullopt;
      } else if (Result.size() > 1 && Index > 1) {
        auto &First = Result[Result.size() - 1];
        auto &Second = Result[Result.size() - 2];
        if (!First.CustomName.empty() || !Second.CustomName.empty()) {
          if (First.CustomName.empty())
            First.CustomName = "unnamed";
          if (Second.CustomName.empty())
            Second.CustomName = "unnamed";
          First.CustomName.append(("+" + Second.CustomName).str());
        }
        auto NewType = buildDoubleType(AllowedRegisters.at(Index - 2),
                                       AllowedRegisters.at(Index - 1),
                                       model::PrimitiveTypeKind::Generic,
                                       TheBinary);
        if (NewType == std::nullopt)
          return std::nullopt;

        First.Type = *NewType;
        Result.pop_back();
      } else {
        return std::nullopt;
      }
    }

    MustUseTheNextOne = MustUseTheNextOne || IsUsed;
  }

  for (auto Pair : llvm::enumerate(llvm::reverse(Result)))
    Pair.value().Index = Pair.index();

  return Result;
}

template<typename CC, typename RegisterType, size_t RegisterCount>
static std::optional<model::QualifiedType>
convertReturnValue(const SortedVector<RegisterType> &UsedRegisters,
                   const RegisterArray<RegisterCount> &AllowedRegisters,
                   model::Binary &TheBinary) {
  bool MustUseTheNextOne = false;
  size_t Size = 0, UsedRegisterCount = 0;
  model::PrimitiveTypeKind::Values Kind = model::PrimitiveTypeKind::Void;
  auto AllowedRegisterRange = llvm::enumerate(llvm::reverse(AllowedRegisters));
  for (auto Pair : AllowedRegisterRange) {
    size_t Index = AllowedRegisters.size() - Pair.index() - 1;
    model::Register::Values Register = Pair.value();
    bool IsUsed = UsedRegisters.find(Register) != UsedRegisters.end();
    if (IsUsed) {
      ++UsedRegisterCount;
      Size += model::Register::getSize(Register);
      if (Kind == model::PrimitiveTypeKind::Void)
        Kind = selectTypeKind(Register);
      else if (Kind != selectTypeKind(Register))
        return std::nullopt;
    } else if (MustUseTheNextOne) {
      if constexpr (!CC::OnlyStartDoubleArgumentsFromAnEvenRegister)
        return std::nullopt;
      else if ((Index & 1) == 0 || UsedRegisterCount <= 1 || Index <= 1)
        return std::nullopt;
    }

    MustUseTheNextOne = MustUseTheNextOne || IsUsed;
  }

  if (UsedRegisterCount > 1 && Kind != model::PrimitiveTypeKind::Float)
    return getType(model::PrimitiveTypeKind::Generic, Size, TheBinary);
  else
    return getType(Kind, Size, TheBinary);
}

template<model::abi::Values V>
std::optional<model::CABIFunctionType>
ABI<V>::toCABI(model::Binary &TheBinary,
               const model::RawFunctionType &Explicit) {
  using CC = CallingConvention;
  static constexpr model::Architecture::Values A = getArchitecture(V);
  if (!verify<A>(Explicit.Arguments, CC::GeneralPurposeArgumentRegisters))
    return std::nullopt;
  if (!verify<A>(Explicit.ReturnValues, CC::GeneralPurposeReturnValueRegisters))
    return std::nullopt;

  model::CABIFunctionType Result;
  Result.ABI = V;

  auto ArgumentList = convertArguments<CC>(Explicit.Arguments,
                                           CC::GeneralPurposeArgumentRegisters,
                                           TheBinary);
  if (ArgumentList == std::nullopt)
    return std::nullopt;
  for (auto &Argument : *ArgumentList)
    Result.Arguments.insert(Argument);

  auto
    ReturnValue = convertReturnValue<CC>(Explicit.ReturnValues,
                                         CC::GeneralPurposeReturnValueRegisters,
                                         TheBinary);
  if (ReturnValue == std::nullopt)
    return std::nullopt;
  Result.ReturnType = *ReturnValue;
  return Result;
}

//
// toRaw
//

template<model::abi::Values V>
std::optional<model::RawFunctionType>
ABI<V>::toRaw(model::Binary &TheBinary,
              const model::CABIFunctionType &Original) {
  return std::nullopt;
}

//
// defaultPrototype
//

template<model::abi::Values V>
model::TypePath ABI<V>::defaultPrototype(model::Binary &TheBinary) {
  model::UpcastableType NewType = model::makeType<model::RawFunctionType>();
  model::TypePath TypePath = TheBinary.recordNewType(std::move(NewType));
  auto &T = *llvm::cast<model::RawFunctionType>(TypePath.get());

  for (const auto &Reg : CallingConvention::GeneralPurposeArgumentRegisters) {
    model::NamedTypedRegister Argument(Reg);
    Argument.Type = buildType(Reg, TheBinary);
    T.Arguments.insert(Argument);
  }

  for (const auto &Rg : CallingConvention::GeneralPurposeReturnValueRegisters) {
    model::TypedRegister ReturnValue(Rg);
    ReturnValue.Type = buildType(Rg, TheBinary);
    T.ReturnValues.insert(ReturnValue);
  }

  for (const auto &Register : CallingConvention::CalleeSavedRegisters)
    T.PreservedRegisters.insert(Register);

  return TypePath;
}

/*

template<model::Architecture::Values Architecture,
         typename RegisterType,
         size_t RegisterCount>
static std::optional<ChosenRegisters<RegisterCount>>
analyze(const SortedVector<RegisterType> &UsedRegisters,
        const RegisterArray<RegisterCount> &AllowedGeneralPurposeRegisters) {
  revng_assert(verify<Architecture>(UsedRegisters));
  revng_assert(verify<Architecture>(AllowedGeneralPurposeRegisters));

  // Ensure all the used registers are allowed
  for (const RegisterType &Register : UsedRegisters) {
    if (llvm::count(AllowedGeneralPurposeRegisters, Register.Location) != 1)
      return std::nullopt;
  }

  ChosenRegisters<RegisterCount> Result;

  // Ensure the register usage continuity, e. g. if the register for the second
  // parameter is used, the register for the first one must be used as well.
  auto SelectUsed = [&UsedRegisters,
                     &Result](const auto &RegisterContainer) -> bool {
    size_t UsedCount = 0;
    bool MustBeUsed = false;
    for (model::Register::Values Register : RegisterContainer) {
      bool IsUsed = UsedRegisters.find(Register) != UsedRegisters.end();
      if (IsUsed)
        Result.emplace_back(Register);

      if (!IsUsed && MustBeUsed)
        return false;

      MustBeUsed = MustBeUsed || IsUsed;
    }

    return true;
  };

  if (!SelectUsed(llvm::reverse(AllowedGeneralPurposeRegisters)))
    return std::nullopt;

  return Result;
}

template<model::abi::Values V>
bool ABI<V>::isCompatible(const model::RawFunctionType &Explicit) {
  static constexpr model::Architecture::Values A = getArchitecture(V);
  using CC = CallingConvention;
  auto ArgumentAnalysis = analyze<A>(Explicit.Arguments,
                                     CC::GeneralPurposeArgumentRegisters);
  auto ReturnValueAnalysis = analyze<A>(Explicit.ReturnValues,
                                        CC::GeneralPurposeReturnValueRegisters);
  return ArgumentAnalysis.has_value() && ReturnValueAnalysis.has_value();
}

template<typename CallingConvention, unsigned AvailableRegisterCount>
static std::optional<model::QualifiedType>
buildReturnType(const ChosenRegisters<AvailableRegisterCount> &Registers,
                model::Binary &Binary) {
  using CC = CallingConvention;

  if (Registers.size() == 0) {
    return model::QualifiedType{
      Binary.getPrimitiveType(model::PrimitiveTypeKind::Void, 0)
    };
  } else if (Registers.size() == 1) {
    return buildType(Registers.front(), Binary);
  } else {
    if constexpr (CC::MaximumGeneralPurposeRegistersPerReturnValue >= 2)
      if (Registers.size() == 2)
        if (auto MaybeType = buildDoubleType(Registers.front(),
                                             Registers.back(),
                                             Binary))
          return *MaybeType;

    if constexpr (CC::AllowPassingAggregatesInRegisters) {
      size_t Offset = 0;
      model::UpcastableType NewType = model::makeType<model::StructType>();
      auto *MultipleReturnValues = llvm::cast<model::StructType>(NewType.get());
      for (const model::Register::Values &Register : Registers) {
        model::StructField NewField;
        NewField.Offset = Offset;
        NewField.Type = buildType(Register, Binary);
        MultipleReturnValues->Fields.insert(std::move(NewField));

        Offset += model::Register::getSize(Register);
      }
      MultipleReturnValues->Size = Offset;

      return model::QualifiedType{ Binary.recordNewType(std::move(NewType)) };
    }

    // Maybe this should be a softer fail, like `return std::nullopt`.
    revng_abort("There shouldn't be more than a single register used for "
                "returning values when ABI doesn't allow splitting "
                "arguments between multiple registers.");
  }
}

template<model::abi::Values V>
std::optional<model::CABIFunctionType>
ABI<V>::toCABI(model::Binary &TheBinary,
               const model::RawFunctionType &Explicit) {

  static constexpr model::Architecture::Values A = getArchitecture(V);
  using CC = CallingConvention;
  auto AnalyzedArguments = analyze<A>(Explicit.Arguments,
                                      CC::GeneralPurposeArgumentRegisters);
  auto
    AnalyzedReturnValues = analyze<A>(Explicit.ReturnValues,
                                      CC::GeneralPurposeReturnValueRegisters);

  if (!AnalyzedArguments.has_value() || !AnalyzedReturnValues.has_value())
    return std::nullopt;

  model::CABIFunctionType Result;
  Result.ABI = V;

  if (auto MaybeType = buildReturnType<CC>(*AnalyzedReturnValues, TheBinary))
    Result.ReturnType = *MaybeType;
  else
    return std::nullopt;

  // Build argument list
  for (size_t Index = 0; auto Register : AnalyzedArguments.value()) {
    model::Argument NewArgument;
    NewArgument.Index = Index++;
    NewArgument.Type = buildType(Register, TheBinary);
    NewArgument.CustomName = Explicit.Arguments.at(Register).CustomName;
    Result.Arguments.insert(std::move(NewArgument));
  }

  return Result;
}

using IndexType = decltype(model::Argument::Index);
using MultiRegister = llvm::SmallVector<model::Register::Values, 1>;
struct InternalArguments {
  llvm::DenseMap<IndexType, MultiRegister> Registers;
  SortedVector<IndexType> Stack;
};
template<typename CallConv>
static std::optional<InternalArguments>
allocateRegistersFreely(const SortedVector<model::Argument> &Arguments) {
  InternalArguments Result;
  size_t UsedGeneralPurposeRegisterCounter = 0;

  using RV = llvm::SmallVector<model::Register::Values, 8>;
  auto SelectGPRs = [&UsedGeneralPurposeRegisterCounter](size_t Size) -> RV {
    size_t ConsideredRegisterCounter = UsedGeneralPurposeRegisterCounter;
    auto HasRunOutOfRegisters = [&ConsideredRegisterCounter]() {
      constexpr auto GPRC = CallConv::GeneralPurposeArgumentRegisters.size();
      return !GPRC || ConsideredRegisterCounter >= GPRC - 1;
    };

    size_t SizeCounter = 0;
    while (SizeCounter < Size && !HasRunOutOfRegisters()) {
      size_t Index = ConsideredRegisterCounter++;
      auto CurrentGPR = CallConv::GeneralPurposeArgumentRegisters[Index];
      SizeCounter += model::Register::getSize(CurrentGPR);
    }

    llvm::SmallVector<model::Register::Values, 8> Result;
    if (SizeCounter >= Size) {
      for (size_t Index = UsedGeneralPurposeRegisterCounter;
           Index < ConsideredRegisterCounter;
           ++Index)
        Result.emplace_back(CallConv::GeneralPurposeArgumentRegisters[Index]);
    }
    return Result;
  };

  using IndexType = decltype(model::Argument::Index);
  constexpr auto Max = CallConv::MaximumGeneralPurposeRegistersPerArgument;
  auto AllocateGPRs = [&](IndexType Index, uint64_t Size) -> size_t {
    auto Registers = SelectGPRs(Size);
    if (!Registers.empty() && Registers.size() <= Max) {
      abi::MultiRegister &Container = Result.Registers[Index];
      for (model::Register::Values &Register : Registers)
        Container.emplace_back(Register);
      return Registers.size();
    }
    return 0;
  };

  for (const model::Argument &Argument : Arguments) {
    std::optional<uint64_t> MaybeSize = Argument.Type.size();
    revng_assert(MaybeSize);

    if (Argument.Type.isScalar()) {
      if (!Argument.Type.isFloat()) {
        if (auto RegisterCount = AllocateGPRs(Argument.Index, *MaybeSize)) {
          UsedGeneralPurposeRegisterCounter += RegisterCount;
          continue;
        }
      } else {
        // TODO: handle floating point arguments.
        return std::nullopt;
      }
    } else if constexpr (CallConv::AllowPassingAggregatesInRegisters) {
      // TODO: verify structs for eligibility.
      if (auto RegisterCount = AllocateGPRs(Argument.Index, *MaybeSize)) {
        UsedGeneralPurposeRegisterCounter += RegisterCount;
        continue;
      }
    }

    Result.Stack.insert(Argument.Index);
  }

  return Result;
}

template<typename CallConv>
static std::optional<InternalArguments>
allocatePositionBasedRegisters(const SortedVector<model::Argument> &Arguments) {
  InternalArguments Result;

  for (const model::Argument &Argument : Arguments) {
    if (Argument.Index < CallConv::GeneralPurposeArgumentRegisters.size()) {
      auto Register = CallConv::GeneralPurposeArgumentRegisters[Argument.Index];
      Result.Registers[Argument.Index].emplace_back(Register);
    } else {
      Result.Stack.insert(Argument.Index);
    }
  }

  return Result;
}

template<typename CallConv>
static std::optional<InternalArguments>
allocateRegisters(const SortedVector<model::Argument> &Arguments) {
  if constexpr (CallConv::ArgumentsArePositionBased)
    return allocatePositionBasedRegisters<CallConv>(Arguments);
  else
    return allocateRegistersFreely<CallConv>(Arguments);
}

template<size_t AvailableRegisterCount>
struct Capacity {
  const size_t NeededRegisterCount;
  const size_t MaximumSizePossible;
};

template<size_t AllowedRegisterLimit, size_t Size>
Capacity<Size>
countCapacity(size_t ValueSize, const RegisterArray<Size> &Registers) {
  size_t RegisterCounter = 0;
  size_t SizeCounter = 0;

  for (const model::Register::Values &Register : Registers) {
    size_t RegisterSize = model::Register::getSize(Register);
    if (SizeCounter < ValueSize)
      ++RegisterCounter;
    SizeCounter += RegisterSize;
    if (RegisterCounter >= AllowedRegisterLimit)
      break;
  }
  if (SizeCounter < ValueSize)
    ++RegisterCounter;

  return Capacity<Size>{ RegisterCounter, SizeCounter };
}

template<model::abi::Values V>
std::optional<model::RawFunctionType>
ABI<V>::toRaw(model::Binary &TheBinary,
              const model::CABIFunctionType &Original) {
  auto Arguments = allocateRegisters<CallingConvention>(Original.Arguments);
  if (!Arguments.has_value())
    return std::nullopt;
  auto [RegisterArguments, StackArguments] = *Arguments;

  model::RawFunctionType Result;
  for (auto [ArgumentIndex, Registers] : RegisterArguments) {
    revng_assert(!Registers.empty());
    auto OriginalName = Original.Arguments.at(ArgumentIndex).CustomName;
    for (size_t Index = 0; auto &Register : Registers) {
      model::Identifier FinalName = OriginalName;
      if (Registers.size() > 1 && !FinalName.empty())
        FinalName += "_part_" + std::to_string(++Index) + "_out_of_"
                     + std::to_string(Registers.size());
      model::NamedTypedRegister Argument(Register);
      Argument.Type = buildType(Register, TheBinary);
      Argument.CustomName = FinalName;
      Result.Arguments.insert(Argument);
    }
  }

  // I'm not sure if this assert is required.
  revng_assert(Result.FinalStackOffset == 0);

  for (auto ArgumentIndex : StackArguments) {
    // Get the argument.
    auto ArgumentIterator = Original.Arguments.find(ArgumentIndex);
    revng_assert(ArgumentIterator != Original.Arguments.end());
    const model::Argument &Argument = *ArgumentIterator;

    // Get it's size.
    auto MaybeSize = Argument.Type.size();
    revng_assert(MaybeSize.has_value());
    size_t Size = *MaybeSize;

    // TODO: handle stack arguments properly.
    // \note: different ABIs could use different stack types.
    // \see: `clrcall` ABI.

    // Compute aligned size.
    constexpr size_t Alignment = CallingConvention::StackAlignment;
    size_t Aligned = Size + Alignment - 1 - (Size + Alignment - 1) % Alignment;
    Result.FinalStackOffset += Aligned;
  }

  constexpr model::Architecture::Values Arch = model::abi::getArchitecture(V);
  constexpr auto PointerSize = model::Architecture::getPointerSize(Arch);
  auto PointerQualifier = model::Qualifier::createPointer(PointerSize);

  // Allocate return values
  if (!Original.ReturnType.isVoid()) {
    if (CallingConvention::GeneralPurposeReturnValueRegisters.empty())
      return std::nullopt;
    auto
      ReturnRegister = CallingConvention::GeneralPurposeReturnValueRegisters[0];
    auto RegisterSize = model::Register::getSize(ReturnRegister);

    auto MaybeReturnValueSize = Original.ReturnType.size();
    revng_assert(MaybeReturnValueSize.has_value());
    using CC = CallingConvention;
    constexpr size_t L = CC::MaximumGeneralPurposeRegistersPerReturnValue;
    auto Capacity = countCapacity<L>(*MaybeReturnValueSize,
                                     CC::GeneralPurposeReturnValueRegisters);

    bool AreSubsequentRegistersAllowed = true;
    if constexpr (!CC::AllowPassingAggregatesInRegisters)
      if (!Original.ReturnType.isScalar())
        AreSubsequentRegistersAllowed = false;

    if (*MaybeReturnValueSize <= Capacity.MaximumSizePossible) {
      if (AreSubsequentRegistersAllowed) {
        constexpr auto
          &AvailableRegisters = CC::GeneralPurposeReturnValueRegisters;
        constexpr size_t AvailableRegisterCount = AvailableRegisters.size();

        revng_assert(Capacity.NeededRegisterCount <= AvailableRegisterCount);
        for (size_t Index = 0; Index < Capacity.NeededRegisterCount; ++Index) {
          auto Register = AvailableRegisters[Index];
          model::QualifiedType Type = buildType(Register, TheBinary);

          model::TypedRegister ReturnPointer(Register);
          ReturnPointer.Type = std::move(Type);
          Result.ReturnValues.insert(ReturnPointer);
        }
      }
    }

    if (Result.ReturnValues.empty()) {
      if (*MaybeReturnValueSize <= RegisterSize
          && AreSubsequentRegistersAllowed) {
        model::TypedRegister Argument(ReturnRegister);
        Argument.Type = buildType(ReturnRegister, TheBinary);
        Result.ReturnValues.insert(Argument);
      } else {
        model::QualifiedType ReturnType = Original.ReturnType;
        ReturnType.Qualifiers.emplace_back(PointerQualifier);

        model::TypedRegister ReturnPointer(ReturnRegister);
        ReturnPointer.Type = std::move(ReturnType);
        Result.ReturnValues.insert(std::move(ReturnPointer));
      }
    }
  }

  // Populate the list of preserved registers
  using CC = CallingConvention;
  for (model::Register::Values Register : CC::CalleeSavedRegisters)
    Result.PreservedRegisters.insert(Register);

  return Result;
}

// TODO: implement this!
template<model::abi::Values V>
void ABI<V>::applyDeductions(RegisterStateMap &Prototype) {
  static_cast<void>(Prototype);
  // using namespace model::RegisterState;
  // using CC = CallingConvention;

  // // Find the highest-indexed YesOrDead argument, and mark YesOrDead all
  // those
  // // before it. Same for return values.
  // bool ArgumentMatch = false;
  // for (auto Register : llvm::reverse(CC::GeneralPurposeArgumentRegisters))
  // {
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
  // for (auto Register :
  // llvm::reverse(CC::GeneralPurposeReturnValueRegisters)) {
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
*/

//
// Specializations
//

template class ABI<model::abi::SystemV_x86_64>;
template class ABI<model::abi::SystemV_x86>;
template class ABI<model::abi::SystemV_x86_regparm_1>;
template class ABI<model::abi::SystemV_x86_regparm_2>;
template class ABI<model::abi::SystemV_x86_regparm_3>;

template class ABI<model::abi::Microsoft_x64>;
template class ABI<model::abi::Microsoft_x64_clrcall>;
template class ABI<model::abi::Microsoft_x64_vectorcall>;

template class ABI<model::abi::Microsoft_x86_clrcall>;
template class ABI<model::abi::Microsoft_x86_vectorcall>;
template class ABI<model::abi::Microsoft_x86_cdecl>;
template class ABI<model::abi::Microsoft_x86_stdcall>;
template class ABI<model::abi::Microsoft_x86_fastcall>;
template class ABI<model::abi::Microsoft_x86_thiscall>;

template class ABI<model::abi::Aarch64>;
template class ABI<model::abi::Aarch32>;

//
// Dynamic convertion.
//

std::optional<model::CABIFunctionType>
convertToCABI(model::abi::Values RuntimeABI,
              model::Binary &TheBinary,
              const model::RawFunctionType &Explicit) {
  using namespace model::abi;
  switch (RuntimeABI) {
  case SystemV_x86_64:
    return ABI<SystemV_x86_64>::toCABI(TheBinary, Explicit);
  case SystemV_x86:
    return ABI<SystemV_x86>::toCABI(TheBinary, Explicit);
  case SystemV_x86_regparm_3:
    return ABI<SystemV_x86_regparm_3>::toCABI(TheBinary, Explicit);
  case SystemV_x86_regparm_2:
    return ABI<SystemV_x86_regparm_2>::toCABI(TheBinary, Explicit);
  case SystemV_x86_regparm_1:
    return ABI<SystemV_x86_regparm_1>::toCABI(TheBinary, Explicit);

  case Microsoft_x64:
    return ABI<Microsoft_x64>::toCABI(TheBinary, Explicit);
  case Microsoft_x64_vectorcall:
    return ABI<Microsoft_x64_vectorcall>::toCABI(TheBinary, Explicit);
  case Microsoft_x64_clrcall:
    return ABI<Microsoft_x64_clrcall>::toCABI(TheBinary, Explicit);
  case Microsoft_x86_cdecl:
    return ABI<Microsoft_x86_cdecl>::toCABI(TheBinary, Explicit);
  case Microsoft_x86_stdcall:
    return ABI<Microsoft_x86_stdcall>::toCABI(TheBinary, Explicit);
  case Microsoft_x86_thiscall:
    return ABI<Microsoft_x86_thiscall>::toCABI(TheBinary, Explicit);
  case Microsoft_x86_fastcall:
    return ABI<Microsoft_x86_fastcall>::toCABI(TheBinary, Explicit);
  case Microsoft_x86_clrcall:
    return ABI<Microsoft_x86_clrcall>::toCABI(TheBinary, Explicit);
  case Microsoft_x86_vectorcall:
    return ABI<Microsoft_x86_vectorcall>::toCABI(TheBinary, Explicit);

  case Aarch64:
    return ABI<Aarch64>::toCABI(TheBinary, Explicit);
  case Aarch32:
    return ABI<Aarch32>::toCABI(TheBinary, Explicit);

  default:
    return ABI<Invalid>::toCABI(TheBinary, Explicit);
  };
}

std::optional<model::RawFunctionType>
convertToRaw(model::abi::Values RuntimeABI,
             model::Binary &TheBinary,
             const model::CABIFunctionType &Original) {
  using namespace model::abi;
  switch (RuntimeABI) {
  case SystemV_x86_64:
    return ABI<SystemV_x86_64>::toRaw(TheBinary, Original);
  case SystemV_x86:
    return ABI<SystemV_x86>::toRaw(TheBinary, Original);
  case SystemV_x86_regparm_3:
    return ABI<SystemV_x86_regparm_3>::toRaw(TheBinary, Original);
  case SystemV_x86_regparm_2:
    return ABI<SystemV_x86_regparm_2>::toRaw(TheBinary, Original);
  case SystemV_x86_regparm_1:
    return ABI<SystemV_x86_regparm_1>::toRaw(TheBinary, Original);

  case Microsoft_x64:
    return ABI<Microsoft_x64>::toRaw(TheBinary, Original);
  case Microsoft_x64_vectorcall:
    return ABI<Microsoft_x64_vectorcall>::toRaw(TheBinary, Original);
  case Microsoft_x64_clrcall:
    return ABI<Microsoft_x64_clrcall>::toRaw(TheBinary, Original);
  case Microsoft_x86_cdecl:
    return ABI<Microsoft_x86_cdecl>::toRaw(TheBinary, Original);
  case Microsoft_x86_stdcall:
    return ABI<Microsoft_x86_stdcall>::toRaw(TheBinary, Original);
  case Microsoft_x86_thiscall:
    return ABI<Microsoft_x86_thiscall>::toRaw(TheBinary, Original);
  case Microsoft_x86_fastcall:
    return ABI<Microsoft_x86_fastcall>::toRaw(TheBinary, Original);
  case Microsoft_x86_clrcall:
    return ABI<Microsoft_x86_clrcall>::toRaw(TheBinary, Original);
  case Microsoft_x86_vectorcall:
    return ABI<Microsoft_x86_vectorcall>::toRaw(TheBinary, Original);

  case Aarch64:
    return ABI<Aarch64>::toRaw(TheBinary, Original);
  case Aarch32:
    return ABI<Aarch32>::toRaw(TheBinary, Original);

  default:
    return ABI<Invalid>::toRaw(TheBinary, Original);
  };
}

} // namespace abi
