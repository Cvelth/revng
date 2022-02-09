#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/ArrayRef.h"

#include "revng/ABI/CallingConventionTrait.h"
#include "revng/Model/ABI.h"
#include "revng/Support/EnumSwitch.h"

constexpr bool areArgumentsPositionBased(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::ArgumentsArePositionBased;
  });
}

constexpr bool
canOnlyStartDoubleArgumentsFromAnEvenRegister(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::OnlyStartDoubleArgumentsFromAnEvenRegister;
  });
}

constexpr bool
canArgumentsBeSplitBetweenRegistersAndStack(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::ArgumentsCanBeSplitBetweenRegistersAndStack;
  });
}

constexpr bool
canOnlyUsePointerToCopyForStackArguments(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::UsePointerToCopyForStackArguments;
  });
}

constexpr size_t countMaximumGPRsPerAggregateArgument(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::MaximumGPRsPerAggregateArgument;
  });
}

constexpr size_t
countMaximumGPRsPerAggregateReturnValue(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::MaximumGPRsPerAggregateReturnValue;
  });
}

constexpr size_t countMaximumGPRsPerScalarArgument(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::MaximumGPRsPerScalarArgument;
  });
}

constexpr size_t countMaximumGPRsPerScalarReturnValue(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::MaximumGPRsPerScalarReturnValue;
  });
}

constexpr llvm::ArrayRef<model::Register::Values>
listGeneralPurposeArgumentRegisters(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  using Registers = llvm::ArrayRef<model::Register::Values>;
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() -> Registers {
    using CC = CallingConventionTrait<A>;
    return CC::GeneralPurposeArgumentRegisters;
  });
}

constexpr llvm::ArrayRef<model::Register::Values>
listGeneralPurposeReturnValueRegisters(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  using Registers = llvm::ArrayRef<model::Register::Values>;
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() -> Registers {
    using CC = CallingConventionTrait<A>;
    return CC::GeneralPurposeReturnValueRegisters;
  });
}

constexpr llvm::ArrayRef<model::Register::Values>
listVectorArgumentRegisters(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  using Registers = llvm::ArrayRef<model::Register::Values>;
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() -> Registers {
    using CC = CallingConventionTrait<A>;
    return CC::VectorArgumentRegisters;
  });
}

constexpr llvm::ArrayRef<model::Register::Values>
listVectorReturnValueRegisters(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  using Registers = llvm::ArrayRef<model::Register::Values>;
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() -> Registers {
    using CC = CallingConventionTrait<A>;
    return CC::VectorReturnValueRegisters;
  });
}

constexpr llvm::ArrayRef<model::Register::Values>
listCalleeSavedRegisters(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  using Registers = llvm::ArrayRef<model::Register::Values>;
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() -> Registers {
    using CC = CallingConventionTrait<A>;
    return CC::CalleeSavedRegisters;
  });
}

constexpr model::Register::Values
getReturnValueLocationRegister(model::ABI::Values ABI) {
  revng_assert(ABI != model::ABI::Invalid);
  return skippingEnumSwitch<1>(ABI, [&]<model::ABI::Values A>() {
    using CC = CallingConventionTrait<A>;
    return CC::ReturnValueLocationRegister;
  });
}
