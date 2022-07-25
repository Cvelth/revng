#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipeline/Kind.h"
#include "revng/Pipes/FunctionKind.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Pipes/RootKind.h"
#include "revng/Pipes/TaggedFunctionKind.h"

namespace revng::pipes {

inline pipeline::Kind Binary("Binary", &rank::Root);

inline RootKind Root("Root", &rank::Root);
inline IsolatedRootKind IsolatedRoot("IsolatedRoot", Root);

inline TaggedFunctionKind
  Isolated("Isolated", &rank::Function, FunctionTags::Isolated);
inline TaggedFunctionKind
  ABIEnforced("ABIEnforced", &rank::Function, FunctionTags::ABIEnforced);
inline TaggedFunctionKind
  CSVsPromoted("CSVsPromoted", &rank::Function, FunctionTags::CSVsPromoted);

inline pipeline::Kind Object("Object", &rank::Root);
inline pipeline::Kind Translated("Translated", &rank::Root);

inline FunctionKind
  FunctionAssemblyInternal("FunctionAssemblyInternal", &rank::Function);
inline FunctionKind
  FunctionAssemblyPTML("FunctionAssemblyPTML", &FunctionsRank);
inline FunctionKind
  FunctionControlFlowGraphSVG("FunctionControlFlowGraphSVG", &FunctionsRank);

} // namespace revng::pipes
