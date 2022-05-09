#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/SortedVector.h"
#include "revng/EarlyFunctionAnalysis/FunctionEdgeType.h"

/* TUPLE-TREE-YAML

name: Edge
type: struct
fields:
  - name: Destination
    type: MetaAddress
  - name: Type
    type: efa::FunctionEdgeType::Values

key:
  - Destination

TUPLE-TREE-YAML */

#include "revng/Yield/Internal/Generated/Early/Edge.h"

namespace yield {

class Edge : public generated::Edge {
public:
  using generated::Edge::Edge;

public:
  bool verify(model::VerifyHelper &VH) const;
  void dump() const debug_function;

public:
  inline bool verify() const debug_function { return verify(false); }
  inline bool verify(bool Assert) const debug_function {
    model::VerifyHelper VH(Assert);
    return verify(VH);
  }
};

} // namespace yield

#include "revng/Yield/Internal/Generated/Late/Edge.h"

