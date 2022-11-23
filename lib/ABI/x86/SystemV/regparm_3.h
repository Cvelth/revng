#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::x86::SystemV::regparm_3 {

constexpr llvm::StringRef Serialized = R"(---
ABI: SystemV_x86_regparm_3

ArgumentsArePositionBased: false
OnlyStartDoubleArgumentsFromAnEvenRegister: false
ArgumentsCanBeSplitBetweenRegistersAndStack: false
UsePointerToCopyForStackArguments: false
CalleeIsResponsibleForStackCleanup: false

StackAlignment: 16

MaximumGPRsPerAggregateArgument: 0
MaximumGPRsPerAggregateReturnValue: 0
MaximumGPRsPerScalarArgument: 1
MaximumGPRsPerScalarReturnValue: 2

GeneralPurposeArgumentRegisters:
  - eax_x86
  - edx_x86
  - ecx_x86
GeneralPurposeReturnValueRegisters:
  - eax_x86
  - edx_x86
VectorArgumentRegisters:
  - xmm0_x86
  - xmm1_x86
  - xmm2_x86
VectorReturnValueRegisters:
  - xmm0_x86
CalleeSavedRegisters:
  - ebx_x86
  - ebp_x86
  - esp_x86
  - edi_x86
  - esi_x86

ReturnValueLocationOnStack: true

TypeSpecific:
  - Size: 1
    AlignAt: 1
  - Size: 2
    AlignAt: 2
  - Size: 4
    AlignAt: 4
  - Size: 8
    AlignAt: 4
)";

Definition Defined = **TupleTree<Definition>::deserialize(Serialized);

} // namespace abi::predefined::x86::SystemV::regparm_3
