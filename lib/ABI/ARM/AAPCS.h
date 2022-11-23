#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::ARM::AAPCS {

constexpr llvm::StringRef Serialized = R"(---
ABI: AAPCS

ArgumentsArePositionBased: false
OnlyStartDoubleArgumentsFromAnEvenRegister: true
ArgumentsCanBeSplitBetweenRegistersAndStack: true
UsePointerToCopyForStackArguments: false
CalleeIsResponsibleForStackCleanup: false

StackAlignment: 4

MaximumGPRsPerAggregateArgument: 4
MaximumGPRsPerAggregateReturnValue: 1
MaximumGPRsPerScalarArgument: 4
MaximumGPRsPerScalarReturnValue: 4

GeneralPurposeArgumentRegisters:
  - r0_arm
  - r1_arm
  - r2_arm
  - r3_arm
GeneralPurposeReturnValueRegisters:
  - r0_arm
  - r1_arm
  - r2_arm
  - r3_arm
VectorArgumentRegisters:
  - q0_arm
  - q1_arm
  - q2_arm
  - q3_arm
VectorReturnValueRegisters:
  - q0_arm
  - q1_arm
  - q2_arm
  - q3_arm
CalleeSavedRegisters:
  - r4_arm
  - r5_arm
  - r6_arm
  - r7_arm
  - r8_arm
  - r10_arm
  - r11_arm
  - r13_arm
  - q4_arm
  - q5_arm
  - q6_arm
  - q7_arm

ReturnValueLocationRegister: r0_arm

TypeSpecific:
  - Size: 1
    AlignAt: 1
  - Size: 2
    AlignAt: 2
  - Size: 4
    AlignAt: 4
  - Size: 8
    AlignAt: 8
)";

Definition Defined = **TupleTree<Definition>::deserialize(Serialized);

} // namespace abi::predefined::ARM::AAPCS
