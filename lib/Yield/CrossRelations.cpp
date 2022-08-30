/// \file CallGraph.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_map>

#include "revng/ADT/STLExtras.h"
#include "revng/Model/Binary.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Yield/CrossRelations.h"

using CR = yield::CrossRelations;
CR::CrossRelations(const SortedVector<efa::FunctionMetadata> &Metadata,
                   const model::Binary &Binary) {
  revng_assert(Metadata.size() == Binary.Functions.size());

  namespace ranks = revng::pipes::ranks;

  // Make sure all the functions are present.
  for (auto Inserter = Relations.batch_insert();
       const auto &Function : Binary.Functions) {
    const auto Location = pipeline::serializedLocation(ranks::Function,
                                                       Function.Entry);
    Inserter.insert(yield::RelationDescription(Location, {}));
  }

  // Make sure all the dynamic functions are present
  for (auto Inserter = Relations.batch_insert();
       const auto &Function : Binary.ImportedDynamicFunctions) {
    const auto Location = pipeline::serializedLocation(ranks::DynamicFunction,
                                                       Function.OriginalName);
    Inserter.insert(yield::RelationDescription(Location, {}));
  }

  for (const auto &[EntryAddress, ControlFlowGraph] : Metadata) {
    for (const auto &BasicBlock : ControlFlowGraph) {
      // TODO: embed information about the call instruction into
      //       `CallLocation` after metadata starts providing it.
      auto CallLocation = pipeline::serializedLocation(ranks::BasicBlock,
                                                       EntryAddress,
                                                       BasicBlock.Start);

      for (const auto &Edge : BasicBlock.Successors) {
        if (auto *CallEdge = llvm::dyn_cast<efa::CallEdge>(Edge.get())) {
          if (const auto &Callee = CallEdge->Destination; Callee.isValid()) {
            auto L = pipeline::serializedLocation(ranks::Function, Callee);
            if (auto It = Relations.find(L); It != Relations.end()) {
              yield::RelationTarget T(yield::RelationType::IsCalledFrom,
                                      CallLocation);
              It->Related.insert(std::move(T));
            }
          } else if (!CallEdge->DynamicFunction.empty()) {
            auto L = pipeline::serializedLocation(ranks::DynamicFunction,
                                                  CallEdge->DynamicFunction);
            if (auto It = Relations.find(L); It != Relations.end()) {
              using namespace yield::RelationType;
              yield::RelationTarget T(IsDynamicallyCalledFrom, CallLocation);
              It->Related.insert(std::move(T));
            }
          } else {
            // Ignore indirect calls.
          }
        } else {
          // Ignore non-call edges.
        }
      }
    }
  }
}

template<typename AddNodeCallable, typename AddEdgeCallable>
static void conversionHelper(const yield::CrossRelations &Input,
                             const AddNodeCallable &AddNode,
                             const AddEdgeCallable &AddEdge) {
  for (const auto &[LocationString, Related] : Input.Relations)
    AddNode(LocationString);

  for (const auto &[LocationString, Related] : Input.Relations) {
    for (const auto &[RelationKind, TargetString] : Related) {
      switch (RelationKind) {
      case yield::RelationType::IsCalledFrom:
        AddEdge(LocationString, TargetString, RelationKind);
        break;

      case yield::RelationType::Invalid:
      case yield::RelationType::Count:
      default:
        revng_abort("Unknown enum value");
      }
    }
  }
}

GenericGraph<yield::CrossRelations::Node, 16, true>
yield::CrossRelations::toGenericGraph() const {
  GenericGraph<yield::CrossRelations::Node, 16, true> Result;

  using NodeView = decltype(Result)::Node *;
  std::unordered_map<std::string_view, NodeView> LookupHelper;
  auto AddNode = [&Result, &LookupHelper](std::string_view Location) {
    auto *Node = Result.addNode(Location);
    auto [Iterator, Success] = LookupHelper.try_emplace(Location, Node);
    revng_assert(Success);
  };
  auto AddEdge = [&LookupHelper](std::string_view From,
                                 std::string_view To,
                                 yield::RelationType::Values Kind) {
    using EL = yield::CrossRelations::EdgeLabel;
    LookupHelper.at(From)->addSuccessor(LookupHelper.at(To), EL{ Kind });
  };
  conversionHelper(*this, AddNode, AddEdge);

  return Result;
}

yield::Graph yield::CrossRelations::toYieldGraph() const {
  yield::Graph Result;

  std::unordered_map<std::string_view, yield::Graph::Node *> LookupHelper;

  namespace ranks = revng::pipes::ranks;
  namespace p = pipeline;
  auto AddNode = [&Result, &LookupHelper](std::string_view Location) {
    auto MaybeAddress = p::genericLocationFromString<0>(Location,
                                                        ranks::Function,
                                                        ranks::BasicBlock,
                                                        ranks::Instruction);
    revng_assert(MaybeAddress.has_value());
    auto [_, Success] = LookupHelper.try_emplace(MaybeAddress->toString(),
                                                 Result.addNode(*MaybeAddress));
    revng_assert(Success);
  };
  auto AddEdge = [&LookupHelper](std::string_view FromLocation,
                                 std::string_view ToLocation,
                                 yield::RelationType::Values) {
    auto FromAddress = p::genericLocationFromString<0>(FromLocation,
                                                       ranks::Function,
                                                       ranks::BasicBlock,
                                                       ranks::Instruction);
    auto ToAddress = p::genericLocationFromString<0>(ToLocation,
                                                     ranks::Function,
                                                     ranks::BasicBlock,
                                                     ranks::Instruction);
    revng_assert(FromAddress.has_value() && ToAddress.has_value());

    auto *FromNode = LookupHelper.at(FromAddress->toString());
    FromNode->addSuccessor(LookupHelper.at(ToAddress->toString()));
  };
  conversionHelper(*this, AddNode, AddEdge);

  return Result;
}
