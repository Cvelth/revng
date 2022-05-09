#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

/* TUPLE-TREE-YAML

name: BasicBlockType
type: enum
members:
  - name: Ordinary
  - name: FallthroughTarget
  - name: Mergeable

TUPLE-TREE-YAML */

#include "revng/Yield/Internal/Generated/Early/BasicBlockType.h"

namespace yield::BasicBlockType {

template<bool ShouldMergeFallthroughTargets>
inline bool shouldSkip(const Values &Type) {
  if (Type == BasicBlockType::Ordinary)
    return false;
  else if (Type == BasicBlockType::FallthroughTarget)
    return ShouldMergeFallthroughTargets;
  else if (Type == BasicBlockType::Mergeable)
    return true;
  else
    revng_abort("Unknown basic block type");

  return false;
}

} // namespace yield::BasicBlockType

#include "revng/Yield/Internal/Generated/Late/BasicBlockType.h"
