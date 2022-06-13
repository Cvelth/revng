#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Yield/FunctionNode.h"

/* TUPLE-TREE-YAML

name: RelationNode
type: struct
fields:
  - name: FunctionWithEntryAt
    type: MetaAddress

  - name: Callees
    sequence:
      type: SortedVector
      elementType: yield::FunctionNode

key:
  - FunctionWithEntryAt

TUPLE-TREE-YAML */

#include "revng/Yield/Generated/Early/RelationNode.h"

namespace yield {

class RelationNode : public generated::RelationNode {
public:
  using generated::RelationNode::RelationNode;
};

} // namespace yield

#include "revng/Yield/Generated/Late/RelationNode.h"
