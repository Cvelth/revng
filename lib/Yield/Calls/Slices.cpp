/// \file Slices.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_map>

#include "llvm/ADT/BreadthFirstIterator.h"
#include "llvm/ADT/PostOrderIterator.h"

#include "revng/Yield/Calls/Slices.h"

using NodeView = const yield::Graph::Node *;

struct NodeLookupHelper {
  const MetaAddress &TargetAddress;
  bool operator()(NodeView Node) { return Node->Address == TargetAddress; }
};

static yield::Graph::Node *
nodeHelper(yield::Graph &Graph, const yield::Graph::Node *Source) {
  auto New = std::make_unique<yield::Graph::Node>(Source->data());
  return Graph.addNode(std::move(New));
}

template<bool IsForwards>
yield::Graph
preservingSliceImpl(const yield::Graph &Input, const MetaAddress &SlicePoint) {
  auto Entry = llvm::find_if(Input.nodes(), NodeLookupHelper{ SlicePoint });
  revng_assert(Entry != Input.nodes().end());

  using NV = std::conditional_t<IsForwards, NodeView, llvm::Inverse<NodeView>>;
  using INV = std::conditional_t<IsForwards, llvm::Inverse<NodeView>, NodeView>;

  // using Trait = llvm::GraphTraits<NV>;
  using InverseTrait = llvm::GraphTraits<INV>;

  // Find the rank of each node, such that for any node its rank is equal to
  // the highest rank among its children plus one.
  std::unordered_map<const yield::Graph::Node *, size_t> Ranks;
  llvm::ReversePostOrderTraversal<NodeView, InverseTrait> RPOT(*Entry);
  for (auto Iterator = RPOT.begin(); Iterator != RPOT.end(); ++Iterator) {
    uint64_t &CurrentRank = Ranks[*Iterator];

    CurrentRank = 0;
    for (auto ChildIterator = InverseTrait::child_begin(*Iterator);
         ChildIterator != InverseTrait::child_end(*Iterator);
         ++ChildIterator) {
      auto RankIterator = Ranks.find(*ChildIterator);
      if (RankIterator != Ranks.end())
        if (RankIterator->second + 1 > CurrentRank)
          CurrentRank = RankIterator->second + 1;
    }
  }

  // For each node, select a single neighbour.
  // TODO: We should choose a better selection algorithm.
  std::unordered_map<NodeView, NodeView> RealEdges;
  for (const yield::Graph::Node *Node : Input.nodes()) {
    auto Helper = [&Ranks, Node](NodeView Neighbour) {
      auto NodeIt = Ranks.find(Node);
      auto NeighbourIt = Ranks.find(Neighbour);
      if (NodeIt == Ranks.end() || NeighbourIt == Ranks.end())
        return false;
      else
        return NodeIt->second > NeighbourIt->second;
    };

    auto It = std::find_if(InverseTrait::child_begin(Node),
                           InverseTrait::child_end(Node),
                           Helper);
    if (It != InverseTrait::child_end(Node)) {
      auto [_, Success] = RealEdges.try_emplace(Node, *It);
      revng_assert(Success);
    } else {
      auto [_, Success] = RealEdges.try_emplace(Node, nullptr);
      revng_assert(Success);
    }
  }

  yield::Graph Result;
  std::unordered_map<NodeView, yield::Graph::Node *> Lookup;
  auto FindOrAddHelper = [&Result, &Lookup](NodeView OldNode) {
    if (auto NewNode = Lookup.find(OldNode); NewNode != Lookup.end())
      return NewNode->second;
    else
      return Lookup.emplace(OldNode, nodeHelper(Result, OldNode)).first->second;
  };

  for (auto NodeIterator = llvm::bf_begin(NV{ *Entry });
       NodeIterator != llvm::bf_end(NV{ *Entry });
       ++NodeIterator) {
    for (auto NeighbourIterator = InverseTrait::child_begin(*NodeIterator);
         NeighbourIterator != InverseTrait::child_end(*NodeIterator);
         ++NeighbourIterator) {
      auto *NewNeighbour = FindOrAddHelper(*NeighbourIterator);
      if (RealEdges.at(*NodeIterator) == *NeighbourIterator) {
        // Emit a real edge.
        NewNeighbour->addSuccessor(FindOrAddHelper(*NodeIterator));
      } else {
        // Emit a fake node.
        auto NewNode = nodeHelper(Result, *NodeIterator);
        NewNode->NextAddress = NewNode->Address;
        NewNeighbour->addSuccessor(NewNode);
      }
    }
  }

  auto EntryIterator = Lookup.find(*Entry);
  if (EntryIterator != Lookup.end())
    Result.setEntryNode(EntryIterator->second);

  return Result;
}

yield::Graph
yield::calls::calleePreservingSlice(const yield::Graph &Input,
                                    const MetaAddress &SlicePoint) {
  return preservingSliceImpl<true>(Input, SlicePoint);
}
yield::Graph
yield::calls::callerPreservingSlice(const yield::Graph &Input,
                                    const MetaAddress &SlicePoint) {
  return preservingSliceImpl<false>(Input, SlicePoint);
}
