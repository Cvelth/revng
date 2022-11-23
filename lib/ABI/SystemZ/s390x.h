#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::SystemZ::s390x {

constexpr llvm::StringRef Serialized = R"(---
ABI: SystemZ_s390x

ArgumentsArePositionBased: true
OnlyStartDoubleArgumentsFromAnEvenRegister: false
ArgumentsCanBeSplitBetweenRegistersAndStack: false
UsePointerToCopyForStackArguments: true
CalleeIsResponsibleForStackCleanup: false

StackAlignment: 8

MaximumGPRsPerAggregateArgument: 1
MaximumGPRsPerAggregateReturnValue: 1
MaximumGPRsPerScalarArgument: 1
MaximumGPRsPerScalarReturnValue: 1

GeneralPurposeArgumentRegisters:
  - r2_systemz
  - r3_systemz
  - r4_systemz
  - r5_systemz
  - r6_systemz
GeneralPurposeReturnValueRegisters:
  - r2_systemz
VectorArgumentRegisters:
  - f0_systemz
  - f1_systemz
  - f2_systemz
  - f3_systemz
  - f4_systemz
  - f5_systemz
  - f6_systemz
  - f7_systemz
# - v24_systemz
# - v26_systemz
# - v28_systemz
# - v30_systemz
# - v25_systemz
# - v27_systemz
# - v29_systemz
# - v31_systemz
VectorReturnValueRegisters:
  - f0_systemz
# - v24_systemz
CalleeSavedRegisters:
  - r6_systemz
  - r7_systemz
  - r8_systemz
  - r9_systemz
  - r10_systemz
  - r11_systemz
  - r12_systemz
  - r13_systemz
  - r15_systemz
  - f8_systemz
  - f9_systemz
  - f10_systemz
  - f11_systemz
  - f12_systemz
  - f13_systemz
  - f15_systemz

ReturnValueLocationRegister: r2_systemz

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
    AlignAt: 8
)";

Definition Defined = **TupleTree<Definition>::deserialize(Serialized);

} // namespace abi::predefined::SystemZ::s390x
