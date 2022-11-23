#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <cstdint>

/* TUPLE-TREE-YAML

name: ScalarType
type: struct
doc: |
  Represents type specific information ABI needs to be aware of,
  for example, alignment.
fields:
  - name: Size
    type: uint64_t

  - name: AlignsAt
    doc: |
      When set to `0` (default), the alignment of this type matches its size
    type: uint64_t
    optional: true

key:
  - Size

TUPLE-TREE-YAML */

#include "revng/ABI/Generated/Early/ScalarType.h"

namespace abi {

class ScalarType : public generated::ScalarType {
public:
  using generated::ScalarType::ScalarType;

  std::uint64_t alignsAt() const {
    revng_assert(Size() != 0);
    return AlignsAt() != 0 ? AlignsAt() : Size();
  }
};

} // namespace abi

#include "revng/ABI/Generated/Late/ScalarType.h"
