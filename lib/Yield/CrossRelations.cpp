/// \file CallGraph.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_map>

#include "revng/ADT/STLExtras.h"
#include "revng/Model/Binary.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Yield/CrossRelations.h"

using CR = yield::CrossRelations;
CR::CrossRelations(const SortedVector<efa::FunctionMetadata> &Metadata,
                   const model::Binary &Binary) {
  revng_assert(Metadata.size() == Binary.Functions.size());

  namespace rank = revng::pipes::rank;

  for (auto Inserter = Relations.batch_insert();
       const auto &Function : Binary.Functions) {
    namespace rank = revng::pipes::rank;
    const auto Location = rank::Function.makeLocation(Function.Entry);
    Inserter.insert(yield::RelationDescription(Location.toString(), {}));
  }

  for (const auto &[EntryAddress, ControlFlowGraph] : Metadata) {
    auto CallLocation = rank::Instruction.makeLocation(EntryAddress,
                                                       MetaAddress::invalid(),
                                                       MetaAddress::invalid());

    for (const auto &BasicBlock : ControlFlowGraph) {
      for (const auto &Edge : BasicBlock.Successors) {
        if (efa::FunctionEdgeType::isCall(Edge->Type)) {
          if (const auto &Callee = Edge->Destination; Callee.isValid()) {
            // TODO: embed information about the call instruction into
            //       `CallLocation` after efa starts providing it.

            auto Location = rank::Function.makeLocation(Callee).toString();
            if (auto It = Relations.find(Location); It != Relations.end()) {
              yield::RelationTarget T(yield::RelationType::IsCalledFrom,
                                      CallLocation.toString());
              It->Related.insert(std::move(T));
            }
          }
        }
      }
    }
  }
}

template<typename AddNodeCallable, typename AddEdgeCallable>
static void convertionHelper(const yield::CrossRelations &Input,
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
  convertionHelper(*this, AddNode, AddEdge);

  return Result;
}

namespace concepts = pipeline::concepts;

// WIP
// TODO: It might make sense to expose this helper interface.
// clang-format off
template <std::size_t Idx, typename ExpectedRank, typename ...SupportedRanks>
  requires (concepts::ConvertibleRank<ExpectedRank, SupportedRanks> && ...)
constexpr std::optional<std::tuple_element_t<Idx, typename ExpectedRank::Tuple>>
genericLocationAccessHelper(std::string_view Serialized,
                            const ExpectedRank &Expected,
                            const SupportedRanks &...Supported) {
  // clang-format on
  std::tuple_element_t<Idx, typename ExpectedRank::Tuple> Result;
  bool ParsedOnce = false;

  using T = std::tuple<ExpectedRank, SupportedRanks...>;
  constexprRepeat<sizeof...(SupportedRanks)>([&, Serialized]<std::size_t I> {
    auto MaybeLoc = std::tuple_element_t<I, T>::locationFromString(Serialized);
    if (MaybeLoc.has_value()) {
      using namespace pipeline;
      Result = std::get<Idx>(Location<ExpectedRank>::convert(*MaybeLoc));

      revng_assert(ParsedOnce == false,
                   "Duplicate supported ranks are not allowed");
      ParsedOnce = true;
    }
  });

  if (ParsedOnce)
    return Result;
  else
    return std::nullopt;
}

yield::Graph yield::CrossRelations::toYieldGraph() const {
  yield::Graph Result;

  std::unordered_map<std::string_view, yield::Graph::Node *> LookupHelper;

  namespace rank = revng::pipes::rank;
  auto AddNode = [&Result, &LookupHelper](std::string_view Location) {
    auto MaybeAddress = genericLocationAccessHelper<0>(Location,
                                                       rank::Function,
                                                       rank::BasicBlock,
                                                       rank::Instruction);
    revng_assert(MaybeAddress.has_value());
    auto [_, Success] = LookupHelper.try_emplace(MaybeAddress->toString(),
                                                 Result.addNode(*MaybeAddress));
    revng_assert(Success);
  };
  auto AddEdge = [&LookupHelper](std::string_view FromLocation,
                                 std::string_view ToLocation,
                                 yield::RelationType::Values) {
    auto FromAddress = genericLocationAccessHelper<0>(FromLocation,
                                                      rank::Function,
                                                      rank::BasicBlock,
                                                      rank::Instruction);
    auto ToAddress = genericLocationAccessHelper<0>(ToLocation,
                                                    rank::Function,
                                                    rank::BasicBlock,
                                                    rank::Instruction);
    revng_assert(FromAddress.has_value() && ToAddress.has_value());

    auto *FromNode = LookupHelper.at(FromAddress->toString());
    FromNode->addSuccessor(LookupHelper.at(ToAddress->toString()));
  };
  convertionHelper(*this, AddNode, AddEdge);

  return Result;
}
