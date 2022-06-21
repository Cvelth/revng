#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipes/Kinds.h"

namespace revng::pipes {

inline pipeline::Kind
  FunctionAssemblyInternal("FunctionAssemblyInternal", &FunctionsRank);
inline pipeline::Kind
  FunctionAssemblyHTML("FunctionAssemblyHTML", &FunctionsRank);
inline pipeline::Kind
  FunctionControlFlowGraphSVG("FunctionControlFlowGraphSVG", &FunctionsRank);

inline pipeline::Kind
  BinaryCallGraphInternal("BinaryCallGraphInternal", &RootRank);
inline pipeline::Kind BinaryCallGraphSVG("BinaryCallGraphSVG", &RootRank);

} // namespace revng::pipes
