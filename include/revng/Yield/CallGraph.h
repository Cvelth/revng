#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/GenericGraph.h"
#include "revng/EarlyFunctionAnalysis/FunctionMetadata.h"
#include "revng/Yield/FunctionNode.h"
#include "revng/Yield/Graph.h"
#include "revng/Yield/RelationNode.h"

/* TUPLE-TREE-YAML

name: CallGraph
type: struct
fields:
  - name: Relations
    sequence:
      type: SortedVector
      elementType: yield::RelationNode

  - name: Roots
    sequence:
      type: SortedVector
      elementType: yield::FunctionNode

  - name: LoopPreventionRoots
    sequence:
      type: SortedVector
      elementType: yield::FunctionNode

TUPLE-TREE-YAML */

#include "revng/Yield/Generated/Early/CallGraph.h"

namespace yield {

class CallGraph : public generated::CallGraph {
public:
  CallGraph() = default;
  CallGraph(const SortedVector<efa::FunctionMetadata> &Metadata,
            const model::Binary &Binary);

  GenericGraph<BidirectionalNode<FunctionNode>, 16, true>
  toGenericGraph() const;
  yield::Graph toYieldGraph() const;
};

} // namespace yield

#include "revng/Yield/Generated/Late/CallGraph.h"
