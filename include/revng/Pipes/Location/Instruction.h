#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipes/Location/BasicBlock.h"
#include "revng/Pipes/Location/Location.h"

/* TUPLE-TREE-YAML

name: Instruction
type: struct
fields:
  - name: Function
    type: MetaAddress
  - name: BasicBlock
    type: MetaAddress
  - name: Address
    type: MetaAddress

TUPLE-TREE-YAML */

#include "revng/Pipes/Location/Generated/Early/Instruction.h"

namespace revng::pipes::location::detail {

class Instruction : public generated::Instruction {
public:
  using generated::Instruction::Instruction;
};

} // namespace revng::pipes::location::detail

#include "revng/Pipes/Location/Generated/Late/Instruction.h"

namespace revng::pipes::location {

using Instruction = detail::Location<detail::Instruction, location::BasicBlock>;

} // namespace revng::pipes::location
