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

inline pipeline::Kind Binary("Binary", &rank::Binary);

inline RootKind Root("Root", &rank::Binary);
inline IsolatedRootKind IsolatedRoot("IsolatedRoot", Root);

inline TaggedFunctionKind
  Isolated("Isolated", &rank::Function, FunctionTags::Isolated);
inline TaggedFunctionKind
  ABIEnforced("ABIEnforced", &rank::Function, FunctionTags::ABIEnforced);
inline TaggedFunctionKind
  CSVsPromoted("CSVsPromoted", &rank::Function, FunctionTags::CSVsPromoted);

inline pipeline::Kind Object("Object", &rank::Binary);
inline pipeline::Kind Translated("Translated", &rank::Binary);

inline FunctionKind
  FunctionAssemblyInternal("FunctionAssemblyInternal", &rank::Function);
inline FunctionKind
  FunctionAssemblyPTML("FunctionAssemblyPTML", &rank::Function);
inline FunctionKind
  FunctionControlFlowGraphSVG("FunctionControlFlowGraphSVG", &rank::Function);

inline pipeline::Kind
  BinaryCrossRelations("BinaryCrossRelations", &rank::Binary);

} // namespace revng::pipes
