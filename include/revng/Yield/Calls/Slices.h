#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Yield/Graph.h"

class MetaAddress;

namespace yield::calls {

Graph vettedSlice(const Graph &Input);
Graph pointSlice(Graph &&Input, const MetaAddress &SlicePoint);
Graph pointSliceWithNoBackedges(Graph &&Input, const MetaAddress &SlicePoint);
Graph pointSliceWithFakeBackedges(Graph &&Input, const MetaAddress &SlicePoint);

} // namespace yield::calls
