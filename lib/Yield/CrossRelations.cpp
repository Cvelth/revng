/// \file CallGraph.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Pipes/Locations.h"
#include "revng/Yield/CrossRelations.h"

using CR = yield::CrossRelations;
CR::CrossRelations(const SortedVector<efa::FunctionMetadata> &Metadata,
                   const model::Binary &Binary) {
  revng_assert(Metadata.size() == Binary.Functions.size());

  for (auto Inserter = Relations.batch_insert();
       const auto &Function : Binary.Functions) {
    namespace l = revng::pipes::location;
    auto S = l::Function.make(Function.Entry).toString();
    Inserter.insert(yield::RelationDescription(std::move(S), {}));
  }

  for (const auto &[EntryAddress, ControlFlowGraph] : Metadata) {
    namespace l = revng::pipes::location;
    auto CallLocation = l::Instruction.make(EntryAddress,
                                            MetaAddress::invalid(),
                                            MetaAddress::invalid());

    for (const auto &BasicBlock : ControlFlowGraph) {
      for (const auto &Edge : BasicBlock.Successors) {
        if (efa::FunctionEdgeType::isCall(Edge->Type)) {
          if (const auto &Callee = Edge->Destination; Callee.isValid()) {
            // TODO: embed information about the call instruction into
            //       `CallLocation` after efa starts providing it.

            auto Iterator = Relations.find(l::Function.make(Callee).toString());
            if (Iterator != Relations.end()) {
              yield::RelationTarget T(yield::RelationType::IsCalledFrom,
                                      CallLocation.toString());
              Iterator->Related.insert(std::move(T));
            }
          }
        }
      }
    }
  }
}

template<IsGenericGraph GenericGraphType>
static GenericGraphType
convertToGenericGraphImpl(const yield::CrossRelations &Relations) {
  GenericGraphType Result;

  revng_abort("WIP");

  return Result;
}

GenericGraph<yield::CrossRelations::Node, 16, true>
yield::CrossRelations::toGenericGraph() const {
  using CustomGraph = GenericGraph<yield::CrossRelations::Node, 16, true>;
  return convertToGenericGraphImpl<CustomGraph>(*this);
}

yield::Graph yield::CrossRelations::toYieldGraph() const {
  return convertToGenericGraphImpl<yield::Graph>(*this);
}
