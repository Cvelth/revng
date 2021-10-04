/// \file ABI.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
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

template<model::Architecture::Values Architecture,
         typename RegisterType,
         size_t AllowedRegisterCount,
         size_t FallbackRegisterCount = 0>
static std::optional<size_t>
analyze(const SortedVector<RegisterType> &UsedRegisters,
        const RegisterArray<AllowedRegisterCount> &AllowedRegisters,
        [[maybe_unused]] const RegisterArray<FallbackRegisterCount>
          &FallbackRegisters = {}) {

  revng_assert(verify<Architecture>(UsedRegisters));
  revng_assert(verify<Architecture>(AllowedRegisters));

  // Ensure all the used registers are allowed
  for (const auto &Register : UsedRegisters)
    if (llvm::count(AllowedRegisters, Register.Location) != 1)
      return std::nullopt;

  // Ensure the register usage continuity, e. g. if the register for the second
  // parameter is used, the register for the first one must be used as well.
  size_t UsedCount = 0;
  bool MustBeUsed = false;
  for (model::Register::Values Register : llvm::reverse(AllowedRegisters)) {
    bool IsUsed = UsedRegisters.count(Register) != 0;

    if (IsUsed)
      ++UsedCount;

    if (!IsUsed && MustBeUsed)
      return std::nullopt;

    MustBeUsed = MustBeUsed || IsUsed;
  }

  return UsedCount;
}

//
// SystemV_x86_64
//

bool ABI<SystemV_x86_64>::isCompatible(model::Binary &TheBinary,
                                       const model::RawFunctionType &Explicit) {
  static constexpr auto Architecture = getArchitecture(SystemV_x86_64);
  auto AreArgumentsCompatible = analyze<Architecture>(Explicit.Arguments,
                                                      ArgumentRegisters);
  auto AreReturnValuesCompatible = analyze<Architecture>(Explicit.ReturnValues,
                                                         ReturnValueRegisters);
  return AreArgumentsCompatible && AreReturnValuesCompatible;
}

std::optional<model::RawFunctionType>
ABI<SystemV_x86_64>::toRaw(model::Binary &TheBinary,
                           const model::CABIFunctionType &Original) {
  using namespace model;

  //
  // Allocate registers
  //
  uint64_t AvailableRegisters = ArgumentRegisters.size();
  VerifyHelper VH;

  for (const Argument &Argument : Original.Arguments) {

    if (not Argument.Type.isScalar())
      return {};

    if (Argument.Type.isFloat())
      return {};

    std::optional<uint64_t> MaybeSize = Argument.Type.size(VH);
    revng_assert(MaybeSize);
    uint64_t Size = *MaybeSize;
    if (Size > AvailableRegisters * 8) {
      // TODO: handle stack arguments
      return {};
    } else {
      AvailableRegisters -= (Size + 7) / 8;
    }
  }

  //
  // Record register arguments
  //
  using namespace model::PrimitiveTypeKind;
  model::RawFunctionType Result;
  auto Primitive = TheBinary.getPrimitiveType(PointerOrNumber, 8);
  QualifiedType Generic64{ Primitive, {} };
  int UsedRegisters = ArgumentRegisters.size() - AvailableRegisters;
  for (int I = 0; I < UsedRegisters; ++I) {
    model::NamedTypedRegister Argument(ArgumentRegisters[I]);
    Argument.Type = Generic64;
    const auto &OriginalArgument = Original.Arguments.at(I);
    Argument.CustomName = OriginalArgument.CustomName;
    Result.Arguments.insert(Argument);
  }

  //
  // Allocate return values
  //
  if (not Original.ReturnType.isVoid()) {

    if (not Original.ReturnType.isScalar())
      return {};

    if (Original.ReturnType.isFloat())
      return {};

    uint64_t AvailableRegisters = ReturnValueRegisters.size();

    std::optional<uint64_t> MaybeSize = Original.ReturnType.size(VH);
    revng_assert(MaybeSize);
    uint64_t Size = *MaybeSize;
    if (Size > AvailableRegisters * 8) {
      // TODO: handle stack arguments
      return {};
    } else {
      AvailableRegisters -= (Size + 7) / 8;
    }

    int UsedRegisters = ReturnValueRegisters.size() - AvailableRegisters;
    for (int I = 0; I < UsedRegisters; ++I) {
      model::TypedRegister Argument(ArgumentRegisters[I]);
      Argument.Type = Generic64;
      Result.ReturnValues.insert(Argument);
    }
  }

  //
  // Populate the list of preserved registers
  //
  for (auto CalleeSavedRegister : CalleeSavedRegisters)
    Result.PreservedRegisters.insert(CalleeSavedRegister);

  return Result;
}

using OptionalCABIFunctionType = std::optional<model::CABIFunctionType>;
OptionalCABIFunctionType
ABI<SystemV_x86_64>::toCABI(model::Binary &TheBinary,
                            const model::RawFunctionType &Explicit) {
  using namespace model;

  auto AnalysisResult = analyze(TheBinary, Explicit);

  if (not AnalysisResult.IsValid)
    return {};

  auto PointerOrNumber = model::PrimitiveTypeKind::PointerOrNumber;
  auto Primitive64 = TheBinary.getPrimitiveType(PointerOrNumber, 8);
  QualifiedType Generic64{ Primitive64, {} };

  auto VoidKind = model::PrimitiveTypeKind::Void;
  auto PrimitiveVoid = TheBinary.getPrimitiveType(VoidKind, 0);
  QualifiedType Void{ PrimitiveVoid, {} };

  model::CABIFunctionType Result;
  Result.ABI = SystemV_x86_64;

  //
  // Build return type
  //
  QualifiedType ReturnType;

  auto ReturnValuesCount = AnalysisResult.ReturnValues;
  if (ReturnValuesCount == 0) {
    ReturnType = Void;
  } else if (ReturnValuesCount == 1) {
    ReturnType = Generic64;
  } else {
    auto NewType = makeType<StructType>();
    auto *MultipleReturnValues = llvm::cast<StructType>(NewType.get());
    MultipleReturnValues->Size = ReturnValuesCount * 8;
    for (uint64_t I = 0; I < AnalysisResult.ReturnValues; ++I) {
      StructField NewField;
      NewField.Offset = I * 8;
      NewField.Type = Generic64;
      MultipleReturnValues->Fields.insert(std::move(NewField));
    }
    ReturnType = QualifiedType{ TheBinary.recordNewType(std::move(NewType)),
                                {} };
  }

  Result.ReturnType = ReturnType;

  //
  // Build argument list
  //
  for (uint64_t I = 0; I < AnalysisResult.Arguments; ++I) {
    Argument NewArgument;
    NewArgument.Index = I;
    NewArgument.Type = Generic64;
    const auto &ExplicitArgument = Explicit.Arguments.at(ArgumentRegisters[I]);
    NewArgument.CustomName = ExplicitArgument.CustomName;
    Result.Arguments.insert(std::move(NewArgument));
  }

  return Result;
}

model::TypePath
ABI<SystemV_x86_64>::defaultPrototype(model::Binary &TheBinary) {
  using namespace model;

  auto NewType = model::makeType<model::RawFunctionType>();
  auto TypePath = TheBinary.recordNewType(std::move(NewType));
  auto &T = *llvm::cast<model::RawFunctionType>(TypePath.get());

  auto PointerOrNumberKind = model::PrimitiveTypeKind::PointerOrNumber;
  auto Primitive64 = TheBinary.getPrimitiveType(PointerOrNumberKind, 8);
  QualifiedType Generic64{ Primitive64, {} };

  for (Register::Values Register : ArgumentRegisters) {
    NamedTypedRegister Argument(Register);
    Argument.Type = Generic64;
    T.Arguments.insert(Argument);
  }

  for (Register::Values Register : ReturnValueRegisters) {
    TypedRegister ReturnValue(Register);
    ReturnValue.Type = Generic64;
    T.ReturnValues.insert(ReturnValue);
  }

  for (Register::Values Register : CalleeSavedRegisters)
    T.PreservedRegisters.insert(Register);

  return TypePath;
}

void ABI<SystemV_x86_64>::applyDeductions(RegisterStateMap &Prototype) {
  using namespace model::RegisterState;

  // Find the highest-indexed YesOrDead argument, and mark YesOrDead all those
  // before it. Same for return values.
  bool ArgumentMatch = false;
  for (auto Register :
       llvm::make_range(ArgumentRegisters.rbegin(), ArgumentRegisters.rend())) {

    auto State = getOrDefault(Prototype,
                              Register,
                              { model::RegisterState::Invalid,
                                model::RegisterState::Invalid });

    auto AsArgument = State.first;

    if (not ArgumentMatch) {
      ArgumentMatch = isYesOrDead(AsArgument);
    } else if (AsArgument != Yes and AsArgument != Dead) {
      Prototype[Register].first = YesOrDead;
    }
  }

  bool ReturnValueMatch = false;
  for (auto Register : llvm::make_range(ReturnValueRegisters.rbegin(),
                                        ReturnValueRegisters.rend())) {

    auto State = getOrDefault(Prototype,
                              Register,
                              { model::RegisterState::Invalid,
                                model::RegisterState::Invalid });

    auto AsReturnValue = State.second;

    if (not ReturnValueMatch) {
      ReturnValueMatch = isYesOrDead(AsReturnValue);
    } else if (AsReturnValue != Yes and AsReturnValue != Dead) {
      Prototype[Register].second = YesOrDead;
    }
  }

  // Mark all the other non-YesOrDead as No
  for (auto &[Register, State] : Prototype) {
    auto &[AsArgument, AsReturnValue] = State;

    if (not isYesOrDead(AsArgument))
      AsArgument = No;

    if (not isYesOrDead(AsReturnValue))
      AsReturnValue = No;
  }
}

//
// Microsoft_x64
//

bool ABI<Microsoft_x64>::isCompatible(model::Binary &TheBinary,
                                      const model::RawFunctionType &Explicit) {
  static constexpr auto Arch = getArchitecture(Microsoft_x64);
  auto AreArgumentsCompatible = analyze<Arch>(Explicit.Arguments,
                                              GeneralArgumentRegisters,
                                              AlternativeArgumentRegisters);
  auto AreReturnValuesCompatible = analyze<Arch>(Explicit.ReturnValues,
                                                 ReturnValueRegisters);
  return AreArgumentsCompatible && AreReturnValuesCompatible;
}

} // namespace abi
