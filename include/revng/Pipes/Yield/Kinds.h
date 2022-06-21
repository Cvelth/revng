#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipes/Kinds.h"

namespace revng::pipes {

inline FunctionKind
  FunctionAssemblyInternal("FunctionAssemblyInternal", &FunctionsRank);
inline FunctionKind
  FunctionAssemblyPTML("FunctionAssemblyPTML", &FunctionsRank);
inline FunctionKind
  FunctionControlFlowGraphSVG("FunctionControlFlowGraphSVG", &FunctionsRank);

} // namespace revng::pipes
