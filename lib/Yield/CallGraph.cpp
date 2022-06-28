/// \file CallGraph.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/Concepts.h"
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
makeStraightforwardGraph(const yield::CallGraph &InternalGraph) {
  GenericGraphType Result;

  std::map<MetaAddress, typename GenericGraphType::Node *> Lookup;
  for (const auto &[Address, _] : InternalGraph.Relations) {
    revng_assert(Address.isValid());
    Lookup.emplace(Address, Result.addNode(Address));
  }

  for (const auto &[Address, Successors] : InternalGraph.Relations) {
    auto FromIterator = Lookup.find(Address);
    revng_assert(FromIterator != Lookup.end());

    for (const auto &[SuccessorAddress] : Successors) {
      auto ToIterator = Lookup.find(SuccessorAddress);
      revng_assert(ToIterator != Lookup.end());

      FromIterator->second->addSuccessor(ToIterator->second);
    }
  }

  Result.setEntryNode(Result.addNode(MetaAddress::invalid()));
  using FN = const yield::FunctionNode;
  auto AllTheRoots = llvm::concat<FN>(InternalGraph.Roots,
                                      InternalGraph.LoopPreventionRoots);
  for (const auto &[RootAddress] : AllTheRoots) {
    auto Iterator = Lookup.find(RootAddress);
    revng_assert(Iterator != Lookup.end());
    Result.getEntryNode()->addSuccessor(Iterator->second);
  }

  return Result;
}

template<IsGenericGraph GenericGraphType>
static GenericGraphType
convertToGenericGraphImpl(const yield::CallGraph &GraphData) {
  auto Result = makeStraightforwardGraph<GenericGraphType>(GraphData);

  // TODO: tree conversion.

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

static auto
addNodeHelper(yield::CallGraph &OutputGraph, const MetaAddress &Address) {
  auto Iterator = OutputGraph.Relations.find(Address);
  if (Iterator == OutputGraph.Relations.end()) {
    yield::RelationNode NewNode(Address);
    return OutputGraph.Relations.insert(std::move(NewNode)).first;
  } else {
    return Iterator;
  }
}

template<bool IsForwards, typename NodeType>
static yield::CallGraph &
addEdgesHelper(yield::CallGraph &OutputGraph, const NodeType &CurrentNode) {
  revng_assert(CurrentNode.FunctionWithEntryAt.isValid());

  auto SuccessorRange = IsForwards ? CurrentNode.successors() :
                                     CurrentNode.predecessors();
  for (const auto &Successor : SuccessorRange) {
    revng_assert(Successor != nullptr);
    const MetaAddress &NextAddress = Successor->FunctionWithEntryAt;

    if (NextAddress.isValid()) {
      addNodeHelper(OutputGraph, NextAddress);

      auto From = OutputGraph.Relations.find(CurrentNode.FunctionWithEntryAt);
      revng_assert(From != OutputGraph.Relations.end());
      if (From->Callees.count(NextAddress) == 0) {
        From->Callees.insert(NextAddress);
        addEdgesHelper<IsForwards>(OutputGraph, *Successor);
      }
    }
  }

  return OutputGraph;
}

template<bool IsForwards>
static yield::CallGraph callGraphSliceImpl(const MetaAddress &SlicePoint,
                                           const yield::CallGraph &Calls) {
  revng_assert(SlicePoint.isValid());

  const auto Generic = Calls.toGenericGraph();

  auto Iterator = llvm::find_if(Generic.nodes(), [&SlicePoint](auto *Node) {
    if (!Node || Node->FunctionWithEntryAt.isInvalid())
      return false;
    else
      return Node->FunctionWithEntryAt == SlicePoint;
  });
  revng_assert(Iterator != Generic.nodes().end(),
               "The slice point is not a known function");

  yield::CallGraph Result;
  Result.Relations.insert(SlicePoint);

  revng_assert(*Iterator != nullptr);
  return addEdgesHelper<IsForwards>(Result, **Iterator);
}

yield::CallGraph
yield::CallGraph::forwardsSlice(const MetaAddress &SlicePoint) const {
  return callGraphSliceImpl<true>(SlicePoint, *this);
}

yield::CallGraph
yield::CallGraph::backwardsSlice(const MetaAddress &SlicePoint) const {
  return callGraphSliceImpl<false>(SlicePoint, *this);
}
