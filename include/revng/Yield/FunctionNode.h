#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Support/MetaAddress.h"

/* TUPLE-TREE-YAML

name: FunctionNode
type: struct
fields:
  - name: FunctionWithEntryAt
    type: MetaAddress

key:
  - FunctionWithEntryAt

TUPLE-TREE-YAML */

#include "revng/Yield/Generated/Early/FunctionNode.h"

namespace yield {

class FunctionNode : public generated::FunctionNode {
public:
  using generated::FunctionNode::FunctionNode;
};

} // namespace yield

#include "revng/Yield/Generated/Late/FunctionNode.h"
