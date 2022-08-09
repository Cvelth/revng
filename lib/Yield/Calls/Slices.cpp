/// \file Slices.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/PostOrderIterator.h"

#include "revng/Yield/Calls/Slices.h"

/// TODO: rework
template<typename NodeT, typename TraitT = NodeT>
void graphToTree(NodeT N0, auto EmitNodeCallable, auto EmitEdgeCallable) {
  using Trait = llvm::GraphTraits<TraitT>;
  using ITrait = llvm::GraphTraits<llvm::Inverse<TraitT>>;
  std::map<NodeT, uint32_t> Ranks;

  if (N0 == nullptr)
    return;

  using NodeRef = typename Trait::NodeRef;
  using SmallPtrs = llvm::SmallPtrSet<typename Trait::NodeRef, 8>;
  using PoIterator = llvm::po_iterator<NodeT, SmallPtrs, false, Trait>;

  std::vector<NodeRef> PO;
  NodeRef EntryNode = Trait::getEntryNode(N0);
  std::copy(PoIterator::begin(EntryNode),
            PoIterator::end(EntryNode),
            std::back_inserter(PO));

  // For each node, store in Ranks[N] the highest rank of all its children
  for (auto NN = PO.rbegin(); NN != PO.rend(); NN++) {
    auto *N = *NN;
    uint32_t Rank = 0;
    uint32_t PredCount = 0;
    for (auto I = ITrait::child_begin(N); I != ITrait::child_end(N); I++) {
      const auto CP = Ranks.find(*I);
      if (CP != Ranks.end())
        Rank = std::max(Rank, CP->second + 1);
      PredCount++;
    }
    Ranks[N] = Rank;
  }

  /* -------------- */

  std::stack<std::pair<NodeT, decltype(Trait::child_begin(N0))>> DFSStack;
  std::set<NodeT> CurrentPath;
  std::vector<NodeT> CurrentPathV;

  DFSStack.emplace(std::make_pair(N0, Trait::child_begin(N0)));
  CurrentPath.insert(N0);
  CurrentPathV.emplace_back(N0);

  while (!DFSStack.empty()) {
    // Each iteration processes one node at time.
    // It starts from N0, it goes to each children.
    // Each visited children enqueues the first level of its own children.

    auto &CurrentNode = DFSStack.top();
    revng_assert(CurrentNode.first != nullptr);
    bool SkipNode = false;

    if (CurrentNode.second == Trait::child_begin(CurrentNode.first)) {
      revng_assert(CurrentPath.size() >= 1);
      if (Ranks[CurrentNode.first] == CurrentPath.size() - 1) {
        // Emit the node as 'Chosen' (with all its children) only if
        // the rank of the node is the path size. TODO: why?
        Ranks[CurrentNode.first] = -1; // mark as visited
        EmitNodeCallable(CurrentNode.first);
        if (CurrentPathV.size() >= 2)
          EmitEdgeCallable(CurrentPathV[CurrentPathV.size() - 2],
                           CurrentPathV.back(),
                           true);
      } else {
        EmitNodeCallable(CurrentNode.first);
        revng_assert(CurrentPathV.size() >= 2);
        EmitEdgeCallable(CurrentPathV[CurrentPathV.size() - 2],
                         CurrentPathV.back(),
                         false);
        SkipNode = true;
      }
    }

    if (SkipNode || CurrentNode.second == Trait::child_end(CurrentNode.first)) {
      // Finished processing children: Pop from DFS Stack.
      CurrentPath.erase(CurrentNode.first);
      revng_assert(CurrentPathV.back() == CurrentNode.first);
      CurrentPathV.pop_back();

      DFSStack.pop();
    } else {
      auto *NewNode = *(CurrentNode.second);
      CurrentNode.second++;

      revng_assert(NewNode != nullptr);
      if (NewNode) {
        auto PP = std::make_pair(NewNode, Trait::child_begin(NewNode));
        if (CurrentPath.find(NewNode) == CurrentPath.end()) {
          DFSStack.emplace(std::move(PP));
          CurrentPath.insert(NewNode);
          CurrentPathV.emplace_back(NewNode);
        }
      }
    }
  }
}

yield::Graph yield::calls::vettedSlice(const yield::Graph &Input) {
  yield::Graph Result;
  auto EmitNodeHelper = [&Result](yield::Graph::Node *Node) {
    Result.addNode(std::make_unique<yield::Graph::Node>(*Node));
  };
  constexpr auto EmitEdgeHelper =
    [](yield::Graph::Node *From, yield::Graph::Node *To, bool Chosen) {
      if (Chosen)
        From->addSuccessor(To);
    };

  graphToTree(Input.getEntryNode(), EmitNodeHelper, EmitEdgeHelper);

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
