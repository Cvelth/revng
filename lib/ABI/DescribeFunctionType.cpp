/// \file ConvertFunctionType.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ABI/ApplyRegisterStateDeductions.h"
#include "revng/ABI/DescribeFunctionType.h"
#include "revng/ABI/Utility.h"
#include "revng/Model/Binary.h"

static model::ABI::Values
chooseABI(model::TypePath Function,
          std::optional<model::ABI::Values> FirstOption,
          model::ABI::Values SecondOption) {
  model::ABI::Values Result = model::ABI::Invalid;
  if (auto Casted = llvm::dyn_cast<model::CABIFunctionType>(Function.get()))
    Result = Casted->ABI;
  if (Result == model::ABI::Invalid && FirstOption.has_value())
    Result = FirstOption.value();
  if (Result == model::ABI::Invalid)
    Result = SecondOption;

  revng_assert(Result != model::ABI::Invalid);
  return Result;
}

static std::optional<DescribedFunction>
describeCABI(DescribedFunction &&Result, model::CABIFunctionType &Function) {

  // Won't show it here since it pretty much mirrors
  // `ConvertionHelper::distributeArguments` (line 550) and
  // `ConvertionHelper::distributeReturnValue` (line 558)
  // from `ConvertFunctionType.cpp`.
  //
  // Function description and convertion would need to be implemented in
  // a single compilation unit to reuse the code (I don't think it's a big deal)

  return std::move(Result);
}

static bool isArgumentYesOrDead(model::Register::Values Register,
                                const RegisterStateMap &Map) {
  const auto &MaybeState = Map[Register].IsUsedForPassingArguments;
  return MaybeState.has_value()
         && model::RegisterState::isYesOrDead(MaybeState.value());
}
static bool isReturnValueYesOrDead(model::Register::Values Register,
                                   const RegisterStateMap &Map) {
  const auto &MaybeState = Map[Register].IsUsedForReturningValues;
  return MaybeState.has_value()
         && model::RegisterState::isYesOrDead(MaybeState.value());
}

static std::optional<DescribedFunction>
describeRaw(DescribedFunction &&Result, model::RawFunctionType &Function) {
  namespace RS = model::RegisterState;

  // Deduce the register state.

  auto RegisterState = makeRegisterStateMap(Result.TheModel.Architecture);
  for (model::NamedTypedRegister Register : Function.Arguments)
    RegisterState[Register.Location].IsUsedForPassingArguments = RS::Yes;
  for (model::TypedRegister Register : Function.ReturnValues)
    RegisterState[Register.Location].IsUsedForReturningValues = RS::Yes;

  auto Deduced = applyRegisterStateDeductions(RegisterState, Result.ABI, true);
  if (!Deduced.has_value())
    return std::nullopt;

  // Add the register arguments in.

  auto GPRs = listGeneralPurposeArgumentRegisters(Result.ABI);
  auto VRs = listVectorArgumentRegisters(Result.ABI);
  if (areArgumentsPositionBased(Result.ABI)) {
    size_t Index = 0;
    for (; Index < std::min(GPRs.size(), VRs.size()); ++Index) {
      bool IsGPRUsed = isArgumentYesOrDead(GPRs[Index], RegisterState);
      bool IsVRUsed = isArgumentYesOrDead(VRs[Index], RegisterState);
      if (IsGPRUsed || IsVRUsed) {
        ArgumentDescription &Argument = Result.Arguments.emplace_back();
        if (IsGPRUsed)
          Argument.Registers.emplace_back(GPRs[Index]);
        if (IsVRUsed)
          Argument.Registers.emplace_back(VRs[Index]);
      }
    }

    for (; Index < GPRs.size(); ++Index) {
      if (isArgumentYesOrDead(GPRs[Index], RegisterState)) {
        ArgumentDescription &Argument = Result.Arguments.emplace_back();
        Argument.Registers.emplace_back(GPRs[Index]);
      }
    }

    for (; Index < VRs.size(); ++Index) {
      if (isArgumentYesOrDead(VRs[Index], RegisterState)) {
        ArgumentDescription &Argument = Result.Arguments.emplace_back();
        Argument.Registers.emplace_back(VRs[Index]);
      }
    }
  } else {
    using CRegister = const model::Register::Values;
    for (CRegister Register : llvm::concat<CRegister>(GPRs, VRs)) {
      if (isArgumentYesOrDead(Register, RegisterState)) {
        ArgumentDescription &Argument = Result.Arguments.emplace_back();
        Argument.Registers.emplace_back(Register);
      }
    }
  }

  // Add the stack arguments in.

  ArgumentDescription::StackSpan StackSpan;
  StackSpan.Offset = Function.FinalStackOffset;
  StackSpan.Size = Function.StackArgumentsType.get()->size().value();

  ArgumentDescription &StackArguments = Result.Arguments.emplace_back();
  StackArguments.Stack = std::move(StackSpan);

  // Set the return values.

  auto RVGPRs = listGeneralPurposeReturnValueRegisters(Result.ABI);
  auto RVVRs = listVectorReturnValueRegisters(Result.ABI);

  using CRegister = const model::Register::Values;
  for (CRegister Register : llvm::concat<CRegister>(RVGPRs, RVVRs))
    if (isReturnValueYesOrDead(Register, RegisterState))
      Result.ReturnValue.Registers.emplace_back(Register);

  return std::move(Result);
}

std::optional<DescribedFunction>
describeFunction(model::TypePath Function,
                 model::Binary &TheModel,
                 std::optional<model::ABI::Values> InputABI) {
  model::ABI::Values ABI = chooseABI(Function, InputABI, TheModel.DefaultABI);
  revng_assert(model::ABI::getArchitecture(ABI) == TheModel.Architecture);

  DescribedFunction Result(Function, TheModel, ABI);
  if (auto CABI = llvm::dyn_cast<model::CABIFunctionType>(Function.get()))
    return describeCABI(std::move(Result), *CABI);
  else if (auto Rw = llvm::dyn_cast<model::RawFunctionType>(Function.get()))
    return describeRaw(std::move(Result), *Rw);
  else
    revng_abort("Only functions can be described.");
}

void DescribedFunction::tryPropagateChangesImpl() {
  /// \todo
}
