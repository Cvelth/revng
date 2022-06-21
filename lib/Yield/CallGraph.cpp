/// \file CallGraph.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Yield/CallGraph.h"

using RelationMapType = decltype(yield::CallGraph::Relations);
using RootSetType = decltype(yield::CallGraph::Roots);

static std::pair<RelationMapType, RelationMapType>
gatherCalls(const SortedVector<efa::FunctionMetadata> &Metadata,
            const model::Binary &Binary) {
  revng_assert(Metadata.size() == Binary.Functions.size());

  std::pair<RelationMapType, RelationMapType> Result;
  for (const auto &Function : Binary.Functions) {
    auto [_0, R] = Result.first.insert(yield::RelationNode(Function.Entry));
    auto [_1, H] = Result.second.insert(yield::RelationNode(Function.Entry));
    revng_assert(R == true && H == true);
  }

  for (const auto &[EntryAddress, ControlFlowGraph] : Metadata) {
    auto CallerIterator = Result.first.find(EntryAddress);
    revng_assert(CallerIterator != Result.first.end());

    for (const auto &BasicBlock : ControlFlowGraph) {
      for (const auto &FunctionEdge : BasicBlock.Successors) {
        if (efa::FunctionEdgeType::isCall(FunctionEdge->Type)) {
          const auto &Destination = FunctionEdge->Destination;
          if (Destination.isValid() && Destination != EntryAddress) {
            auto FunctionIterator = Binary.Functions.find(Destination);
            if (FunctionIterator != Binary.Functions.end()) {
              auto MetadataIterator = Metadata.find(Destination);
              if (MetadataIterator != Metadata.end()) {
                CallerIterator->Callees.insert(Destination);

                auto HelperIterator = Result.second.find(Destination);
                if (HelperIterator != Result.second.end())
                  HelperIterator->Callees.insert(EntryAddress);
              }
            }
          }
        }
      }
    }
  }

  return Result;
}

static void cutTree(const MetaAddress &RootNodeOfTheTreeToCutOut,
                    RootSetType &TheSetOfAllTheNodes,
                    const RelationMapType &NodeRelations) {
  revng_assert(NodeRelations.count(RootNodeOfTheTreeToCutOut) != 0);

  auto Iterator = TheSetOfAllTheNodes.find(RootNodeOfTheTreeToCutOut);
  if (Iterator == TheSetOfAllTheNodes.end())
    return;

  TheSetOfAllTheNodes.erase(Iterator);

  auto NodeIterator = NodeRelations.find(RootNodeOfTheTreeToCutOut);
  revng_assert(NodeIterator != NodeRelations.end());

  for (const yield::FunctionNode &Successor : NodeIterator->Callees)
    cutTree(Successor.FunctionWithEntryAt, TheSetOfAllTheNodes, NodeRelations);
}

static std::pair<RootSetType, RootSetType>
gatherRoots(const RelationMapType &Relations, const RelationMapType &Helpers) {
  revng_assert(Relations.size() == Helpers.size());

  // Initialize all the nodes as possible roots.
  RootSetType PossibleRoots;
  for (auto [Address, _] : Relations)
    PossibleRoots.insert(Address);

  std::pair<RootSetType, RootSetType> Result;

  // Detect definite roots - the nodes that have no predecessors.
  RootSetType &DefiniteRoots = Result.first;
  for (auto [Address, Neighbors] : Helpers)
    if (Neighbors.empty())
      DefiniteRoots.insert(Address);

  // Cut the trees beginning from definite roots nodes from possible ones.
  for (const auto &RootAddress : DefiniteRoots)
    cutTree(RootAddress.FunctionWithEntryAt, PossibleRoots, Relations);

  // Possible roots now only contain cycles.
  // Arbitrarily choose some nodes as roots to break those.
  RootSetType &LoopPrevention = Result.second;
  while (!PossibleRoots.empty()) {
    auto Arbitrary = *PossibleRoots.begin();
    auto [_, Success] = LoopPrevention.insert(Arbitrary.FunctionWithEntryAt);
    revng_assert(Success, "Duplicate roots.");
    cutTree(Arbitrary.FunctionWithEntryAt, PossibleRoots, Relations);
  }

  revng_assert(PossibleRoots.empty());
  return Result;
}

yield::CallGraph::CallGraph(const SortedVector<efa::FunctionMetadata> &Metadata,
                            const model::Binary &Binary) {
  RelationMapType Helpers;
  std::tie(Relations, Helpers) = gatherCalls(Metadata, Binary);
  std::tie(Roots, LoopPreventionRoots) = gatherRoots(Relations, Helpers);
}

template<IsGenericGraph GenericGraphType>
static GenericGraphType
convertToGenericGraphImpl(const yield::CallGraph &Graph) {
  GenericGraphType Result;

  revng_abort("WIP");

  return Result;
}

GenericGraph<BidirectionalNode<yield::FunctionNode>, 16, true>
yield::CallGraph::toGenericGraph() const {
  using CustomGraph = GenericGraph<BidirectionalNode<FunctionNode>, 16, true>;
  return convertToGenericGraphImpl<CustomGraph>(*this);
}

yield::Graph yield::CallGraph::toYieldGraph() const {
  return convertToGenericGraphImpl<yield::Graph>(*this);
}
