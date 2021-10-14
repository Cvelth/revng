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
  for (const model::Register::Values &Register : AllowedRegisters) {
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
  for (const RegisterType &Register : UsedRegisters) {
    // Check if the registers of the same architecture are used.
    if (model::Register::getArchitecture(Register.Location) != Architecture)
      return false;
  }

  return true;
}

template<model::Architecture::Values Architecture,
         typename RegisterType,
         size_t RegisterCount>
static std::optional<llvm::SmallVector<model::Register::Values, RegisterCount>>
analyze(const SortedVector<RegisterType> &UsedRegisters,
        const RegisterArray<RegisterCount> &AllowedGenericRegisters) {
  revng_assert(verify<Architecture>(UsedRegisters));
  revng_assert(verify<Architecture>(AllowedGenericRegisters));

  // Ensure all the used registers are allowed
  for (const RegisterType &Register : UsedRegisters) {
    if (llvm::count(AllowedGenericRegisters, Register.Location) != 1)
      return std::nullopt;
  }

  llvm::SmallVector<model::Register::Values, RegisterCount> Result;

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

  if (!SelectUsed(llvm::reverse(AllowedGenericRegisters)))
    return std::nullopt;

  return Result;
}

template<model::abi::Values V>
bool ABI<V>::isCompatible(const model::RawFunctionType &Explicit) {
  static constexpr model::Architecture::Values A = getArchitecture(V);
  using CC = CallingConvention;
  auto ArgumentAnalysis = analyze<A>(Explicit.Arguments,
                                     CC::GenericArgumentRegisters);
  auto ReturnValueAnalysis = analyze<A>(Explicit.ReturnValues,
                                        CC::GenericReturnValueRegisters);
  return ArgumentAnalysis.has_value() && ReturnValueAnalysis.has_value();
}

constexpr static model::PrimitiveTypeKind::Values
selectTypeKind(model::Register::Values) {
  // TODO
  return model::PrimitiveTypeKind::PointerOrNumber;
}

static model::QualifiedType
buildType(model::Register::Values Register, model::Binary &TheBinary) {
  model::PrimitiveTypeKind::Values Kind = selectTypeKind(Register);
  size_t Size = model::Register::getSize(Register);
  return model::QualifiedType{ TheBinary.getPrimitiveType(Kind, Size) };
}

template<model::abi::Values V>
std::optional<model::CABIFunctionType>
ABI<V>::toCABI(model::Binary &TheBinary,
               const model::RawFunctionType &Explicit) {
  static constexpr model::Architecture::Values A = getArchitecture(V);
  using CC = CallingConvention;
  auto AnalyzedArguments = analyze<A>(Explicit.Arguments,
                                      CC::GenericArgumentRegisters);
  auto AnalyzedReturnValues = analyze<A>(Explicit.ReturnValues,
                                         CC::GenericReturnValueRegisters);

  if (!AnalyzedArguments.has_value() || !AnalyzedReturnValues.has_value())
    return std::nullopt;

  model::CABIFunctionType Result;
  Result.ABI = V;

  // Build return type
  if (AnalyzedReturnValues->size() == 0) {
    Result.ReturnType = model::QualifiedType{
      TheBinary.getPrimitiveType(model::PrimitiveTypeKind::Void, 0)
    };
  } else if (AnalyzedReturnValues->size() == 1) {
    Result.ReturnType = buildType(AnalyzedReturnValues->front(), TheBinary);
  } else {
    if constexpr (!CC::AggregateArgumentsCanOccupySubsequentRegisters) {
      if constexpr (!CC::PrimitiveArgumentsCanOccupySubsequentRegisters) {
        // Maybe this should be a softer fail, like `return std::nullopt`.
        revng_abort("There shouldn't be more than a single register used for "
                    "returning values when ABI doesn't allow splitting "
                    "arguments between multiple registers.");
      }
    }

    size_t Offset = 0;
    model::UpcastableType NewType = model::makeType<model::StructType>();
    auto *MultipleReturnValues = llvm::cast<model::StructType>(NewType.get());
    for (model::Register::Values &Register : AnalyzedReturnValues.value()) {
      model::StructField NewField;
      NewField.Offset = Offset;
      NewField.Type = buildType(Register, TheBinary);
      MultipleReturnValues->Fields.insert(std::move(NewField));

      Offset += model::Register::getSize(Register);
    }
    MultipleReturnValues->Size = Offset;

    Result.ReturnType = { TheBinary.recordNewType(std::move(NewType)), {} };
  }

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
allocateRegisters(const SortedVector<model::Argument> &Arguments) {
  InternalArguments Result;
  size_t UsedGenericRegisterCounter = 0;

  using OR = std::optional<model::Register::Values>;
  auto GetTheNextGenericRegister = [&UsedGenericRegisterCounter]() -> OR {
    if (UsedGenericRegisterCounter < CallConv::GenericArgumentRegisters.size())
      return CallConv::GenericArgumentRegisters[UsedGenericRegisterCounter];
    else
      return std::nullopt;
  };

  using RV = llvm::SmallVector<model::Register::Values, 8>;
  auto GetGenericRegisters = [&UsedGenericRegisterCounter](size_t Size) -> RV {
    size_t ConsideredRegisterCounter = UsedGenericRegisterCounter;
    auto HasRunOutOfRegisters = [&ConsideredRegisterCounter]() {
      constexpr auto RegisterCount = CallConv::GenericArgumentRegisters.size();
      return !RegisterCount || ConsideredRegisterCounter >= RegisterCount - 1;
    };

    size_t SizeCounter = 0;
    while (SizeCounter < Size && !HasRunOutOfRegisters()) {
      auto Rg = CallConv::GenericArgumentRegisters[ConsideredRegisterCounter++];
      SizeCounter += model::Register::getSize(Rg);
    }

    llvm::SmallVector<model::Register::Values, 8> Result;
    if (SizeCounter >= Size) {
      for (size_t Index = UsedGenericRegisterCounter;
           Index < ConsideredRegisterCounter;
           ++Index)
        Result.emplace_back(CallConv::GenericArgumentRegisters[Index]);
    }
    return Result;
  };

  for (const model::Argument &Argument : Arguments) {
    std::optional<uint64_t> MaybeSize = Argument.Type.size();
    revng_assert(MaybeSize);

    if constexpr (CallConv::AggregateArgumentsCanOccupySubsequentRegisters
                  || CallConv::PrimitiveArgumentsCanOccupySubsequentRegisters) {
      auto Registers = GetGenericRegisters(*MaybeSize);
      if (Registers.empty()) {
        Result.Stack.insert(Argument.Index);
        continue;
      } else {
        abi::MultiRegister &Container = Result.Registers[Argument.Index];
        for (model::Register::Values &Register : Registers)
          Container.emplace_back(Register);
        UsedGenericRegisterCounter += Registers.size();
      }
    } else {
      auto NextRegister = GetTheNextGenericRegister();
      if (!NextRegister.has_value()) {
        Result.Stack.insert(Argument.Index);
        continue;
      }

      size_t NextSize = model::Register::getSize(*NextRegister);
      if (*MaybeSize > NextSize) {
        Result.Stack.insert(Argument.Index);
      } else {
        Result.Registers[Argument.Index].emplace_back(*NextRegister);
        ++UsedGenericRegisterCounter;
      }
    }
  }

  return Result;
}

template<size_t AvailableRegisterCount>
struct Capacity {
  const size_t NeededRegisterCount;
  const size_t MaximumSizePossible;
};

template<size_t Size>
Capacity<Size>
countCapacity(size_t ValueSize, const RegisterArray<Size> &Registers) {
  size_t RegisterCounter = 0;
  size_t SizeCounter = 0;

  for (const model::Register::Values &Register : Registers) {
    size_t RegisterSize = model::Register::getSize(Register);
    if (SizeCounter < ValueSize)
      ++RegisterCounter;
    SizeCounter += RegisterSize;
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
  for (auto ArgumentIndex : StackArguments) {
    // TODO: handle stack arguments.
    // \note: different ABIs could use different stack types.
    // \see: `clrcall` ABI.
  }

  constexpr model::Architecture::Values Arch = model::abi::getArchitecture(V);
  constexpr auto PointerSize = model::Architecture::getPointerSize(Arch);
  auto PointerQualifier = model::Qualifier::createPointer(PointerSize);

  // Allocate return values
  if (!Original.ReturnType.isVoid()) {
    if (CallingConvention::GenericReturnValueRegisters.empty())
      return std::nullopt;

    auto MaybeReturnValueSize = Original.ReturnType.size();
    revng_assert(MaybeReturnValueSize.has_value());
    using CC = CallingConvention;
    auto Capacity = countCapacity(*MaybeReturnValueSize,
                                  CC::GenericReturnValueRegisters);

    constexpr bool A = CC::AggregateArgumentsCanOccupySubsequentRegisters;
    constexpr bool P = CC::PrimitiveArgumentsCanOccupySubsequentRegisters;
    bool IsScalar = Original.ReturnType.isScalar();
    bool AreSubsequentRegistersAllowed = (IsScalar && P) || (!IsScalar && A);
    if (AreSubsequentRegistersAllowed) {
      constexpr auto &AvailableRegisters = CC::GenericReturnValueRegisters;
      constexpr size_t AvailableRegisterCount = AvailableRegisters.size();

      if (Original.ReturnType.size() > Capacity.MaximumSizePossible) {
        revng_assert(AvailableRegisterCount != 0);
        auto ReturnValueRegister = AvailableRegisters[0];

        const model::Type *Pointer = Original.ReturnType.UnqualifiedType.get();
        const model::Type::Key &ReturnTypeKey = Pointer->key();
        if (TheBinary.Types.find(ReturnTypeKey) == TheBinary.Types.end())
          TheBinary.recordNewType(Original.ReturnType);

        model::QualifiedType ReturnType = Original.ReturnType;
        ReturnType.Qualifiers.emplace_back(PointerQualifier);

        model::TypedRegister ReturnPointer(ReturnValueRegister);
        ReturnPointer.Type = std::move(ReturnType);
        Result.ReturnValues.insert(std::move(ReturnPointer));
      } else {
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
      auto ReturnRegister = CallingConvention::GenericReturnValueRegisters[0];
      auto RegisterSize = model::Register::getSize(ReturnRegister);
      if (*MaybeReturnValueSize > RegisterSize)
        return std::nullopt;

      model::TypedRegister Argument(ReturnRegister);
      Argument.Type = buildType(ReturnRegister, TheBinary);
      Result.ReturnValues.insert(Argument);
    }
  }

  // Populate the list of preserved registers
  using CC = CallingConvention;
  for (model::Register::Values Register : CC::CalleeSavedRegisters)
    Result.PreservedRegisters.insert(Register);

  return Result;
}

template<model::abi::Values V>
model::TypePath ABI<V>::defaultPrototype(model::Binary &TheBinary) {
  model::UpcastableType NewType = model::makeType<model::RawFunctionType>();
  model::TypePath TypePath = TheBinary.recordNewType(std::move(NewType));
  auto &T = *llvm::cast<model::RawFunctionType>(TypePath.get());

  for (const auto &Register : CallingConvention::GenericArgumentRegisters) {
    model::NamedTypedRegister Argument(Register);
    Argument.Type = buildType(Register, TheBinary);
    T.Arguments.insert(Argument);
  }

  for (const auto &Register : CallingConvention::GenericReturnValueRegisters) {
    model::TypedRegister ReturnValue(Register);
    ReturnValue.Type = buildType(Register, TheBinary);
    T.ReturnValues.insert(ReturnValue);
  }

  for (const auto &Register : CallingConvention::CalleeSavedRegisters)
    T.PreservedRegisters.insert(Register);

  return TypePath;
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
  // for (auto Register : llvm::reverse(CC::GenericArgumentRegisters))
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
  // llvm::reverse(CC::GenericReturnValueRegisters)) {
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
// More specializations are to come

} // namespace abi
