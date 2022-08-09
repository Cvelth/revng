/// \file Slices.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Yield/Calls/Slices.h"

yield::Graph yield::calls::vettedSlice(const yield::Graph &Input) {
  return yield::Graph{};
}

yield::Graph yield::calls::pointSlice(yield::Graph &&InputGraph,
                                      const MetaAddress &SlicePoint) {
  return yield::Graph{};
}

yield::Graph
yield::calls::pointSliceWithNoBackedges(yield::Graph &&Input,
                                        const MetaAddress &SlicePoint) {
  return yield::Graph{};
}

yield::Graph
yield::calls::pointSliceWithFakeBackedges(yield::Graph &&Input,
                                          const MetaAddress &SlicePoint) {
  return yield::Graph{};
}
