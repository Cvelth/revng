#pragma once

//
// This file is distributed under the MIT Licence. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/ABI/Definition.h"

namespace abi::predefined::x86::Microsoft::clrcall {

constexpr llvm::StringRef Serialized = R"(---
ABI: Microsoft_x86_clrcall

ArgumentsArePositionBased: false
OnlyStartDoubleArgumentsFromAnEvenRegister: false
ArgumentsCanBeSplitBetweenRegistersAndStack: false
UsePointerToCopyForStackArguments: false
CalleeIsResponsibleForStackCleanup: false

StackAlignment: 4

MaximumGPRsPerAggregateArgument: 0
MaximumGPRsPerAggregateReturnValue: 0
MaximumGPRsPerScalarArgument: 0
MaximumGPRsPerScalarReturnValue: 0
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
    AlignAt: 8

# \note: it's probably a good idea to double-check most of this.
)";

Definition Defined = **TupleTree<Definition>::deserialize(Serialized);

} // namespace abi::predefined::x86::Microsoft::clrcall
