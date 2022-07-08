#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipes/Kinds.h"

namespace revng::pipes {

inline FunctionKind FunctionAssemblyInternal("FunctionAssemblyInternal",
                                             &FunctionsRank,
                                             { &location::Function,
                                               &location::BasicBlock,
                                               &location::Instruction });

inline FunctionKind FunctionAssemblyPTML("FunctionAssemblyPTML",
                                         &FunctionsRank,
                                         { &location::Function });

inline FunctionKind FunctionControlFlowGraphSVG("FunctionControlFlowGraphSVG",
                                                &FunctionsRank,
                                                { &location::Function });

inline pipeline::Kind BinaryCrossRelations("BinaryCrossRelations",
                                           &RootRank,
                                           { &location::Binary });

} // namespace revng::pipes
