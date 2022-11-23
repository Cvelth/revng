#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::x86_64::SystemV::Default {

constexpr llvm::StringRef Serialized = R"(---
ABI: SystemV_x86_64

ArgumentsArePositionBased: false
OnlyStartDoubleArgumentsFromAnEvenRegister: false
ArgumentsCanBeSplitBetweenRegistersAndStack: false
UsePointerToCopyForStackArguments: false
CalleeIsResponsibleForStackCleanup: false

StackAlignment: 16

# \note: if an aggregate argument contains SSE-fields, this doesn't apply.
MaximumGPRsPerAggregateArgument: 2
MaximumGPRsPerAggregateReturnValue: 2
MaximumGPRsPerScalarArgument: 8
MaximumGPRsPerScalarReturnValue: 2

GeneralPurposeArgumentRegisters:
  - rdi_x86_64
  - rsi_x86_64
  - rdx_x86_64
  - rcx_x86_64
  - r8_x86_64
  - r9_x86_64
GeneralPurposeReturnValueRegisters:
  - rax_x86_64
  - rdx_x86_64
VectorArgumentRegisters:
  - xmm0_x86_64
  - xmm1_x86_64
  - xmm2_x86_64
  - xmm3_x86_64
  - xmm4_x86_64
  - xmm5_x86_64
  - xmm6_x86_64
  - xmm7_x86_64
VectorReturnValueRegisters:
  - xmm0_x86_64
  - xmm1_x86_64
CalleeSavedRegisters:
  - rbx_x86_64
  - rbp_x86_64
  - rsp_x86_64
  - r12_x86_64
  - r13_x86_64
  - r14_x86_64
  - r15_x86_64
  - fs_x86_64

ReturnValueLocationRegister: rdi_x86_64

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

} // namespace abi::predefined::x86_64::SystemV::Default
