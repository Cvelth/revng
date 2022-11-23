#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <vector>

#include "revng/ABI/TypeSpecific.h"
#include "revng/ADT/SortedVector.h"
#include "revng/Model/ABI.h"
#include "revng/Model/Register.h"

/* TUPLE-TREE-YAML

name: Definition
type: struct
fields:
  - name: ABI
    type: model::ABI::Values

  - name: ArgumentsArePositionBased
    type: bool
  - name: OnlyStartDoubleArgumentsFromAnEvenRegister
    type: bool
  - name: ArgumentsCanBeSplitBetweenRegistersAndStack
    type: bool
  - name: UsePointerToCopyForStackArguments
    type: bool
  - name: CalleeIsResponsibleForStackCleanup
    type: bool

  - name: StackAlignment
    type: uint64_t
  - name: MaximumGPRsPerAggregateArgument
    type: uint64_t
  - name: MaximumGPRsPerAggregateReturnValue
    type: uint64_t
  - name: MaximumGPRsPerScalarArgument
    type: uint64_t
  - name: MaximumGPRsPerScalarReturnValue
    type: uint64_t

  - name: GeneralPurposeArgumentRegisters
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: GeneralPurposeReturnValueRegisters
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: VectorArgumentRegisters
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: VectorReturnValueRegisters
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: CalleeSavedRegisters
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true

  - name: ReturnValueLocationRegister
    type: model::Register::Values
    optional: true
  - name: ReturnValueLocationOnStack
    type: bool
    optional: true

  - name: TypeSpecific
    sequence:
      type: SortedVector
      elementType: TypeSpecific
    optional: true

key:
  - ABI

TUPLE-TREE-YAML */

#include "revng/ABI/Generated/Early/Definition.h"

namespace abi {

class Definition : public generated::Definition {
public:
  using generated::Definition::Definition;
};

} // namespace abi

#include "revng/ABI/Generated/Late/Definition.h"
