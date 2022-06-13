/// \file CallGraph.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Yield/CallGraph.h"

yield::CallGraph::CallGraph(const SortedVector<efa::FunctionMetadata> &Metadata,
                            const model::Binary &Binary) {
  revng_abort("WIP");
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
