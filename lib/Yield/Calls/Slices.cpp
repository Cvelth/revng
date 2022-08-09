/// \file Slices.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_map>

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/PostOrderIterator.h"

#include "revng/Yield/Calls/Slices.h"

yield::Graph yield::calls::vettedSlice(const yield::Graph &Input) {
  if (Input.size() == 0)
    return {};

  revng_assert(Input.getEntryNode() != nullptr);
  const yield::Graph::Node *Entry = Input.getEntryNode();

  // For each node, store in Ranks[N] the highest rank of all its children
  std::unordered_map<const yield::Graph::Node *, uint64_t> Ranks;
  llvm::ReversePostOrderTraversal<const yield::Graph::Node *> RPOT(Entry);
  for (auto Iterator = RPOT.begin(); Iterator != RPOT.end(); ++Iterator) {
    uint64_t &CurrentRank = Ranks[*Iterator];

    CurrentRank = 0;
    for (auto *Child : (*Iterator)->predecessors()) {
      auto ChildIterator = Ranks.find(Child);
      if (ChildIterator != Ranks.end())
        if (ChildIterator->second + 1 > CurrentRank)
          CurrentRank = ChildIterator->second + 1;
    }
  }

  yield::Graph Result;
  std::unordered_map<const yield::Graph::Node *, yield::Graph::Node *> Lookup;
  for (auto NodeIt = llvm::df_begin(Entry); NodeIt != llvm::df_end(Entry);) {
    revng_assert(NodeIt.getPathLength() >= 1);
    const auto *Node = *NodeIt;
    if (Ranks.at(Node) == NodeIt.getPathLength() - 1) {
      revng_assert(!Lookup.contains(Node));

      auto New = std::make_unique<yield::Graph::Node>(Node->data());
      auto [NewIt, _] = Lookup.emplace(Node, Result.addNode(std::move(New)));
      if (NodeIt.getPathLength() > 1) {

        // Emit the node as 'Chosen' (with all its children) only if
        // the rank of the node is the path size. TODO: why?

        auto ChildIt = Lookup.find(NodeIt.getPath(NodeIt.getPathLength() - 2));
        revng_assert(ChildIt != Lookup.end());
        ChildIt->second->addSuccessor(NewIt->second);
      }

      ++NodeIt;
    } else {
      NodeIt.skipChildren();
    }
  }

  auto EntryIterator = Lookup.find(Entry);
  revng_assert(EntryIterator != Lookup.end());
  Result.setEntryNode(EntryIterator->second);

  return Result;
}

yield::Graph yield::calls::pointSlice(yield::Graph &&InputGraph,
                                      const MetaAddress &SlicePoint) {
  return yield::Graph{};
}

yield::Graph
yield::calls::pointSliceWithNoBackedges(yield::Graph &&Input,
                                        const MetaAddress &SlicePoint) {
  return yield::Graph{};
}

yield::Graph
yield::calls::pointSliceWithFakeBackedges(yield::Graph &&Input,
                                          const MetaAddress &SlicePoint) {
  return yield::Graph{};
}
