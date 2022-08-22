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

static yield::Graph::Node *
nodeHelper(yield::Graph &Graph, const yield::Graph::Node *Source) {
  auto New = std::make_unique<yield::Graph::Node>(Source->data());
  return Graph.addNode(std::move(New));
}

/// \tparam NV local `NodeView` specialization
/// \tparam INV inverted location `NodeView` specialization
template<typename NV, typename INV>
yield::Graph
preservingSliceImpl(const yield::Graph &Input, const MetaAddress &SlicePoint) {
  auto LookupHelper = [&SlicePoint](NodeView Node) {
    return Node->Address == SlicePoint;
  };
  auto Entry = llvm::find_if(Input.nodes(), LookupHelper);
  revng_assert(Entry != Input.nodes().end());

  // Find the rank of each node, such that for any node its rank is equal to
  // the highest rank among its children plus one.
  std::unordered_map<const yield::Graph::Node *, size_t> Ranks;
  std::vector<NodeView> PostOrder(llvm::po_begin(NV{ *Entry }),
                                  llvm::po_end(NV{ *Entry }));
  for (NodeView CurrentNode : llvm::reverse(PostOrder)) {
    uint64_t &CurrentRank = Ranks[CurrentNode];

    CurrentRank = 0;
    for (auto Child : llvm::children<INV>(CurrentNode))
      if (auto RankIterator = Ranks.find(Child); RankIterator != Ranks.end())
        CurrentRank = std::max(RankIterator->second + 1, CurrentRank);
  }

  // For each node, select a single predecessor to keep connected to.
  // The ranks calculated earlier are used to choose a specific one.
  //
  // TODO: We should consider a better selection algorithm.
  std::unordered_map<NodeView, NodeView> RealEdges;
  for (NodeView Node : Input.nodes()) {
    NodeView SelectedNeighbour = nullptr;

    // This selects a child with the highest rank
    auto NodeIt = Ranks.find(Node);
    if (NodeIt != Ranks.end()) {
      size_t SelectedNeighbourRank = 0;
      for (NodeView Neighbour : llvm::children<INV>(Node)) {
        auto NeighbourIt = Ranks.find(Neighbour);
        if (NeighbourIt != Ranks.end()) {

          // Select the neighbour with the highest possible rank that is still
          // lower than the current node's rank.
          size_t Rank = NeighbourIt->second;
          if (Rank < NodeIt->second && Rank >= SelectedNeighbourRank) {
            SelectedNeighbour = Neighbour;
            SelectedNeighbourRank = Rank;
          }
        }
      }
    }

    auto [_, Success] = RealEdges.try_emplace(Node, SelectedNeighbour);
    revng_assert(Success);
  }

  yield::Graph Result;
  std::unordered_map<NodeView, yield::Graph::Node *> Lookup;

  // Returns the version of the node from the new graph if it exists,
  // or adds a new one to if it does not.
  auto FindOrAddHelper = [&Result, &Lookup](NodeView OldNode) {
    if (auto NewNode = Lookup.find(OldNode); NewNode != Lookup.end())
      return NewNode->second;
    else
      return Lookup.emplace(OldNode, nodeHelper(Result, OldNode)).first->second;
  };

  // Make sure entry node is always present so that this never returns an empty
  // graph
  FindOrAddHelper(*Entry);

  // Fill in the `Result` graph.
  for (NodeView Node : llvm::breadth_first(NV{ *Entry })) {
    for (auto Neighbour : llvm::children<INV>(Node)) {
      if (Ranks.contains(Neighbour)) {
        auto *NewNeighbour = FindOrAddHelper(Neighbour);
        if (RealEdges.at(Node) == Neighbour) {
          // Emit a real edge, if this is the neighbour selected earlier.
          NewNeighbour->addSuccessor(FindOrAddHelper(Node));
        } else {
          // Emit a fake node otherwise.
          auto NewNode = nodeHelper(Result, Node);
          NewNode->NextAddress = NewNode->Address;
          NewNeighbour->addSuccessor(NewNode);
        }
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
  // Forwards direction, makes sure no successor relation ever gets lost.
  return preservingSliceImpl<NodeView, llvm::Inverse<NodeView>>(Input,
                                                                SlicePoint);
}
yield::Graph
yield::calls::callerPreservingSlice(const yield::Graph &Input,
                                    const MetaAddress &SlicePoint) {
  // Backwards direction, makes sure no predecessor relation ever gets lost.
  return preservingSliceImpl<llvm::Inverse<NodeView>, NodeView>(Input,
                                                                SlicePoint);
}
