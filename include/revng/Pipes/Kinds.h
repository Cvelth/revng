#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipeline/Kind.h"
#include "revng/Pipes/FunctionKind.h"
#include "revng/Pipes/Locations.h"
#include "revng/Pipes/RootKind.h"
#include "revng/Pipes/TaggedFunctionKind.h"

namespace revng::pipes {

inline pipeline::Rank RootRank("root");

inline pipeline::Rank FunctionsRank("function", RootRank);

inline pipeline::Kind Binary("Binary", &RootRank, { &location::Binary });

inline RootKind Root("Root", &RootRank);
inline IsolatedRootKind IsolatedRoot("IsolatedRoot", Root);

inline TaggedFunctionKind
  Isolated("Isolated", &FunctionsRank, FunctionTags::Isolated);
inline TaggedFunctionKind
  ABIEnforced("ABIEnforced", &FunctionsRank, FunctionTags::ABIEnforced);
inline TaggedFunctionKind
  CSVsPromoted("CSVsPromoted", &FunctionsRank, FunctionTags::CSVsPromoted);

inline pipeline::Kind Object("Object", &RootRank, { &location::Binary });
inline pipeline::Kind
  Translated("Translated", &RootRank, { &location::Binary });

} // namespace revng::pipes
