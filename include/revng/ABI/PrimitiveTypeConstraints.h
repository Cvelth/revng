#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <cstdint>

/* TUPLE-TREE-YAML

name: PrimitiveTypeConstraints
type: struct
doc: |
  Represents type specific constraints ABI might need to be aware of,
  for example, alignment.
fields:
  - name: Size
    type: uint64_t

  - name: AlignAt
    type: uint64_t

key:
  - Size

TUPLE-TREE-YAML */

// TODO: this is to be extended in the future, mainly with vector-register
// specific information.

#include "revng/ABI/Generated/Early/PrimitiveTypeConstraints.h"

namespace abi {

class PrimitiveTypeConstraints : public generated::PrimitiveTypeConstraints {
public:
  using generated::PrimitiveTypeConstraints::PrimitiveTypeConstraints;
};

} // namespace abi

#include "revng/ABI/Generated/Late/PrimitiveTypeConstraints.h"
