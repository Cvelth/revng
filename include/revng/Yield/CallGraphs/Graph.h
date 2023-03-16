#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Support/BasicBlockID.h"
#include "revng/Yield/Support/GraphLayout/Graphs.h"

namespace yield::calls {

struct Node {
  BasicBlockID Address;
  BasicBlockID NextAddress;

  explicit Node(BasicBlockID Address = BasicBlockID::invalid()) :
    Address(std::move(Address)) {}
};

struct Edge {
  bool IsBackwards = false;
};

using PreLayoutGraph = layout::InputGraph<Node, Edge>;
using PreLayoutNode = PreLayoutGraph::Node;

using PostLayoutGraph = layout::OutputGraph<Node, Edge>;
using PostLayoutNode = PostLayoutGraph::Node;

} // namespace yield::calls
