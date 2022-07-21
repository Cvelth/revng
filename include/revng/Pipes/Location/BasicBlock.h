#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipes/Location/Function.h"
#include "revng/Pipes/Location/Location.h"

/* TUPLE-TREE-YAML

name: BasicBlock
type: struct
fields:
  - name: Function
    type: MetaAddress
  - name: Start
    type: MetaAddress

TUPLE-TREE-YAML */

#include "revng/Pipes/Location/Generated/Early/BasicBlock.h"

namespace revng::pipes::location::detail {

class BasicBlock : public generated::BasicBlock {
public:
  using generated::BasicBlock::BasicBlock;
};

} // namespace revng::pipes::location::detail

#include "revng/Pipes/Location/Generated/Late/BasicBlock.h"

namespace revng::pipes::location {

using BasicBlock = detail::Location<detail::BasicBlock, location::Function>;

} // namespace revng::pipes::location
