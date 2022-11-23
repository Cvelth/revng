#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::MIPSEL::SystemV::o32 {

constexpr llvm::StringRef Serialized = R"(---
ABI: SystemV_MIPSEL_o32

ArgumentsArePositionBased: true
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
  - a0_mips
  - a1_mips
  - a2_mips
  - a3_mips
GeneralPurposeReturnValueRegisters:
  - v0_mips
  - v1_mips
VectorArgumentRegisters:
  - f12_mips
  - f13_mips
  - f14_mips
  - f15_mips
VectorReturnValueRegisters:
  - f0_mips
  - f1_mips
  - f2_mips
  - f3_mips
CalleeSavedRegisters:
  - s0_mips
  - s1_mips
  - s2_mips
  - s3_mips
  - s4_mips
  - s5_mips
  - s6_mips
  - s7_mips
  - gp_mips
  - sp_mips
  - fp_mips
  - f20_mips
  - f21_mips
  - f22_mips
  - f23_mips
  - f24_mips
  - f25_mips
  - f26_mips
  - f27_mips
  - f28_mips
  - f29_mips
  - f30_mips
  - f31_mips

ReturnValueLocationRegister: v0_mips

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

} // namespace abi::predefined::MIPSEL::SystemV::o32
