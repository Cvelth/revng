#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::AArch64::AAPCS64 {

constexpr llvm::StringRef Serialized = R"(---
ABI: AAPCS64

ArgumentsArePositionBased: false
OnlyStartDoubleArgumentsFromAnEvenRegister: true
ArgumentsCanBeSplitBetweenRegistersAndStack: true
UsePointerToCopyForStackArguments: false
CalleeIsResponsibleForStackCleanup: false

StackAlignment: 16

MaximumGPRsPerAggregateArgument: 2
MaximumGPRsPerAggregateReturnValue: 2
MaximumGPRsPerScalarArgument: 2
MaximumGPRsPerScalarReturnValue: 2

GeneralPurposeArgumentRegisters:
  - x0_aarch64
  - x1_aarch64
  - x2_aarch64
  - x3_aarch64
  - x4_aarch64
  - x5_aarch64
  - x6_aarch64
  - x7_aarch64
GeneralPurposeReturnValueRegisters:
  - x0_aarch64
  - x1_aarch64
  - x2_aarch64
  - x3_aarch64
  - x4_aarch64
  - x5_aarch64
  - x6_aarch64
  - x7_aarch64
VectorArgumentRegisters:
  - v0_aarch64
  - v1_aarch64
  - v2_aarch64
  - v3_aarch64
  - v4_aarch64
  - v5_aarch64
  - v6_aarch64
  - v7_aarch64
VectorReturnValueRegisters:
  - v0_aarch64
  - v1_aarch64
  - v2_aarch64
  - v3_aarch64
  - v4_aarch64
  - v5_aarch64
  - v6_aarch64
  - v7_aarch64
CalleeSavedRegisters:
  - x19_aarch64
  - x20_aarch64
  - x21_aarch64
  - x22_aarch64
  - x23_aarch64
  - x24_aarch64
  - x25_aarch64
  - x26_aarch64
  - x27_aarch64
  - x28_aarch64
  - x29_aarch64
  - sp_aarch64
  - v8_aarch64
  - v9_aarch64
  - v10_aarch64
  - v11_aarch64
  - v12_aarch64
  - v13_aarch64
  - v14_aarch64
  - v15_aarch64

ReturnValueLocationRegister: x8_aarch64

TypeSpecific:
  - Size: 1
    AlignAt: 1
  - Size: 2
    AlignAt: 2
  - Size: 4
    AlignAt: 4
  - Size: 8
    AlignAt: 8
  - Size: 16
    AlignAt: 16
)";

Definition Defined = **TupleTree<Definition>::deserialize(Serialized);

} // namespace abi::predefined::AArch64::AAPCS64
