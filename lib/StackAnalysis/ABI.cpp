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

using IndexType = decltype(model::Argument::Index);
struct DistributedArgument {
  llvm::SmallVector<model::Register::Values, 1> Registers = {};
  size_t Size = 0, SizeOnStack = 0;
};
using DistributedArguments = llvm::DenseMap<IndexType, DistributedArgument>;

using ArgumentContainer = SortedVector<model::Argument>;
template<typename CallConv>
static std::optional<DistributedArguments>
distributeArgumentsBasedOnPositions(const ArgumentContainer &Arguments) {
  DistributedArguments Result;

  for (const model::Argument &Argument : Arguments) {
    auto &DistributedRef = Result[Argument.Index];

    auto MaybeSize = Argument.Type.size();
    if (MaybeSize == std::nullopt)
      return std::nullopt;
    DistributedRef.Size = *MaybeSize;

    if (Argument.Type.isFloat()) {
      if (Argument.Index < CallConv::VectorArgumentRegisters.size()) {
        auto Register = CallConv::VectorArgumentRegisters[Argument.Index];
        DistributedRef.Registers.emplace_back(Register);
      } else {
        DistributedRef.SizeOnStack = DistributedRef.Size;
      }
    } else {
      if (Argument.Index < CallConv::GeneralPurposeArgumentRegisters.size()) {
        auto Reg = CallConv::GeneralPurposeArgumentRegisters[Argument.Index];
        DistributedRef.Registers.emplace_back(Reg);
      } else {
        DistributedRef.SizeOnStack = DistributedRef.Size;
      }
    }
  }

  return Result;
}

template<typename CallConv, size_t RegisterCount>
std::pair<DistributedArgument, size_t>
considerRegisters(size_t Size,
                  size_t AllowedRegisterLimit,
                  size_t OccupiedRegisterCount,
                  const RegisterArray<RegisterCount> &AllowedRegisters,
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

  if constexpr (CallConv::OnlyStartDoubleArgumentsFromAnEvenRegister) {
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

  return { DA, ConsideredRegisterCounter };
}

template<typename CallConv>
static std::optional<DistributedArguments>
distributeArgumentsFreely(const SortedVector<model::Argument> &Arguments) {
  DistributedArguments Result;
  size_t UsedGeneralPurposeRegisterCounter = 0;
  size_t UsedVectorRegisterCounter = 0;

  for (const model::Argument &Argument : Arguments) {
    auto MaybeSize = Argument.Type.size();
    if (MaybeSize == std::nullopt)
      return std::nullopt;

    using CC = CallConv;
    constexpr bool SplitArgs = CC::ArgumentsCanBeSplitBetweenRegistersAndStack;
    if (Argument.Type.isFloat()) {
      const auto &Registers = CC::VectorArgumentRegisters;
      size_t &Counter = UsedVectorRegisterCounter;
      const size_t Limit = 1;

      auto [Distributed, NextIndex] = considerRegisters<CC>(*MaybeSize,
                                                            Limit,
                                                            Counter,
                                                            Registers,
                                                            SplitArgs);
      Result[Argument.Index] = Distributed;
      Counter = NextIndex;
    } else {
      const auto &Registers = CC::GeneralPurposeArgumentRegisters;
      size_t &Counter = UsedGeneralPurposeRegisterCounter;
      if (Argument.Type.isScalar()) {
        const size_t Limit = CC::MaxGeneralPurposeRegistersPerPrimitiveArgument;

        auto [Distributed, NextIndex] = considerRegisters<CC>(*MaybeSize,
                                                              Limit,
                                                              Counter,
                                                              Registers,
                                                              SplitArgs);
        Result[Argument.Index] = Distributed;
        Counter = NextIndex;
      } else {
        const size_t Limit = CC::MaxGeneralPurposeRegistersPerAggregateArgument;

        auto [Distributed, NextIndex] = considerRegisters<CC>(*MaybeSize,
                                                              Limit,
                                                              Counter,
                                                              Registers,
                                                              SplitArgs);
        Result[Argument.Index] = Distributed;
        Counter = NextIndex;
      }
    }
  }

  return Result;
}

template<typename CallConv>
static std::optional<DistributedArguments>
distributeArguments(const SortedVector<model::Argument> &Arguments) {
  if constexpr (CallConv::ArgumentsArePositionBased)
    return distributeArgumentsBasedOnPositions<CallConv>(Arguments);
  else
    return distributeArgumentsFreely<CallConv>(Arguments);
}

template<typename CallConv>
static std::optional<DistributedArgument>
distributeReturnValue(const model::QualifiedType &ReturnValueType) {
  auto MaybeSize = ReturnValueType.size();
  if (MaybeSize == std::nullopt)
    return std::nullopt;

  using CC = CallConv;
  if (ReturnValueType.isFloat()) {
    const auto &Registers = CC::VectorReturnValueRegisters;
    return considerRegisters<CC>(*MaybeSize, 1, 0, Registers, false).first;
  } else {
    const auto &Registers = CC::GeneralPurposeReturnValueRegisters;
    if (ReturnValueType.isScalar()) {
      const size_t L = CC::MaxGeneralPurposeRegistersPerPrimitiveReturnValue;
      return considerRegisters<CC>(*MaybeSize, L, 0, Registers, false).first;
    } else {
      const size_t L = CC::MaxGeneralPurposeRegistersPerAggregateReturnValue;
      return considerRegisters<CC>(*MaybeSize, L, 0, Registers, false).first;
    }
  }
}

template<model::abi::Values V>
std::optional<model::RawFunctionType>
ABI<V>::toRaw(model::Binary &TheBinary,
              const model::CABIFunctionType &Original) {
  auto Arguments = distributeArguments<CallingConvention>(Original.Arguments);
  if (Arguments == std::nullopt)
    return std::nullopt;

  model::RawFunctionType Result;
  for (auto [ArgumentIndex, ArgumentStorage] : *Arguments) {
    if (!ArgumentStorage.Registers.empty()) {
      // Handle the registers
      auto OriginalName = Original.Arguments.at(ArgumentIndex).CustomName;
      for (size_t Index = 0; auto Register : ArgumentStorage.Registers) {
        auto FinalName = OriginalName;
        if (ArgumentStorage.Registers.size() > 1 && !FinalName.empty())
          FinalName += "_part_" + std::to_string(++Index) + "_out_of_"
                       + std::to_string(ArgumentStorage.Registers.size());
        model::NamedTypedRegister Argument(Register);
        Argument.Type = buildType(Register, TheBinary);
        Argument.CustomName = FinalName;
        Result.Arguments.insert(Argument);
      }
    }

    if (ArgumentStorage.SizeOnStack != 0) {
      // Handle the stack
      auto ArgumentIterator = Original.Arguments.find(ArgumentIndex);
      revng_assert(ArgumentIterator != Original.Arguments.end());
      const model::Argument &Argument = *ArgumentIterator;

      auto MaybeSize = Argument.Type.size();
      revng_assert(MaybeSize.has_value());
      size_t Size = *MaybeSize;

      // TODO: handle stack arguments properly.
      // \note: different ABIs could use different stack types.
      // \see: `clrcall` ABI.

      constexpr size_t Alignment = CallingConvention::StackAlignment;
      size_t Aligned = Size + Alignment - 1
                       - (Size + Alignment - 1) % Alignment;
      Result.FinalStackOffset += Aligned;
    }
  }

  if (!Original.ReturnType.isVoid()) {
    using CC = CallingConvention;
    auto ReturnValue = distributeReturnValue<CC>(Original.ReturnType);
    if (ReturnValue == std::nullopt)
      return std::nullopt;

    if (!ReturnValue->Registers.empty()) {
      // Handle a register-based return value.
      for (model::Register::Values Register : ReturnValue->Registers) {
        constexpr auto GenericKind = model::PrimitiveTypeKind::Generic;
        auto RegisterSize = model::Register::getSize(Register);
        auto GenericType = getType(GenericKind, RegisterSize, TheBinary);

        model::TypedRegister ReturnValue;
        ReturnValue.Location = Register;
        ReturnValue.Type = GenericType;

        Result.ReturnValues.insert(std::move(ReturnValue));
      }
    } else {
      // Handle a pointer-based return value.
      if (CallingConvention::GeneralPurposeReturnValueRegisters.empty())
        return std::nullopt;

      auto Register = CallingConvention::GeneralPurposeReturnValueRegisters[0];
      auto RegisterSize = model::Register::getSize(Register);
      auto PointerQualifier = model::Qualifier::createPointer(RegisterSize);

      auto MaybeReturnValueSize = Original.ReturnType.size();
      if (MaybeReturnValueSize == std::nullopt)
        return std::nullopt;
      if (ReturnValue->Size != *MaybeReturnValueSize)
        return std::nullopt;

      model::QualifiedType ReturnType = Original.ReturnType;
      ReturnType.Qualifiers.emplace_back(PointerQualifier);

      model::TypedRegister ReturnPointer(Register);
      ReturnPointer.Type = std::move(ReturnType);
      Result.ReturnValues.insert(std::move(ReturnPointer));
    }
  }

  // Populate the list of preserved registers
  using CC = CallingConvention;
  for (model::Register::Values Register : CC::CalleeSavedRegisters)
    Result.PreservedRegisters.insert(Register);

  return Result;
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
