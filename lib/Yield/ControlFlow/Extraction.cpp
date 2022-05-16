/// \file Extraction.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/EarlyFunctionAnalysis/FunctionEdgeType.h"
#include "revng/Model/Binary.h"
#include "revng/Yield/ControlFlow/Extraction.h"
#include "revng/Yield/ControlFlow/FallthroughDetection.h"
#include "revng/Yield/Internal/Function.h"
#include "revng/Yield/Internal/Graph.h"

struct NodeLookupHelper {
  MetaAddress Key;
  yield::Node *Node = nullptr;
};

template<>
struct KeyedObjectTraits<NodeLookupHelper> {
  static MetaAddress key(const NodeLookupHelper &Object) { return Object.Key; }
  static NodeLookupHelper fromKey(const MetaAddress &Key) { return { Key }; }
};

yield::Graph yield::cfg::extractFromInternal(const yield::Function &Function,
                                             const model::Binary &Binary) {
  SortedVector<NodeLookupHelper> LookupTable;

  // Create a node in the graph for each of the basic blocks.
  Graph Result;
  for (auto Inserter = LookupTable.batch_insert();
       auto &Block : Function.BasicBlocks) {
    if (Block.Type != yield::BasicBlockType::Mergeable) {
      revng_assert(Block.Address.isValid());

      auto *CurrentBlockNode = Result.addNode();
      CurrentBlockNode->Address = Block.Address;

      Inserter.insert({ Block.Address, CurrentBlockNode });
    }
  }

  // Add all the direct edges into the graph.
  for (const auto &Block : Function.BasicBlocks) {
    if (!yield::BasicBlockType::shouldSkip<false>(Block.Type)) {
      const yield::BasicBlock *Last = &Block;
      auto Next = detectFallthrough<false>(Block, Function, Binary);
      while (Next != nullptr)
        Next = detectFallthrough<false>(*(Last = Next), Function, Binary);

      auto From = LookupTable.find(Block.Address);
      revng_assert(From != LookupTable.end());

      From->Node->NextAddress = Last->NextAddress;

      llvm::SmallVector<yield::Node::EdgeView, 4> TargetEdges;
      for (const MetaAddress &Target : Last->Targets) {
        auto From = LookupTable.find(Block.Address);
        revng_assert(From != LookupTable.end());
        if (Target.isValid()) {
          if (auto To = LookupTable.find(Target); To != LookupTable.end()) {
            auto EdgeView = From->Node->addSuccessor(To->Node);
            revng_assert(EdgeView.Label != nullptr);
            TargetEdges.emplace_back(EdgeView);
          }
        }
      }

      // Colour taken and refused edges.
      if (Last->Targets.size() == 2) {
        revng_assert(TargetEdges.size() < 3);
        if (TargetEdges.size() == 2) {
          auto &Front = TargetEdges.front();
          auto &Back = TargetEdges.back();
          if (Front.Neighbor->Address == Last->NextAddress) {
            Front.Label->Type = yield::Graph::EdgeType::Refused;
            Back.Label->Type = yield::Graph::EdgeType::Taken;
          } else if (Back.Neighbor->Address == Last->NextAddress) {
            Front.Label->Type = yield::Graph::EdgeType::Taken;
            Back.Label->Type = yield::Graph::EdgeType::Refused;
          }
        } else if (TargetEdges.size() == 1) {
          auto &Front = TargetEdges.front();
          if (Front.Neighbor->Address == Last->NextAddress)
            TargetEdges.front().Label->Type = yield::Graph::EdgeType::Refused;
        }
      }
    }
  }

  // Add a root node.
  auto EntryIterator = LookupTable.find(Function.Address);
  revng_assert(EntryIterator != LookupTable.end());
  auto *RootNode = Result.addNode();
  RootNode->Address = MetaAddress::invalid();
  RootNode->addSuccessor(EntryIterator->Node);
  Result.setEntryNode(RootNode);

  return Result;
}
