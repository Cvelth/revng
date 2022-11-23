#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <cstdint>

/* TUPLE-TREE-YAML

name: TypeSpecific
type: struct
doc: |
  Represent an extra type specific information ABI might need to be aware of
  for example, alignment.

  TODO: this is to be extended in the future, mainly with vector-register
  specific information.
fields:
  - name: Size
    type: uint64_t

  - name: AlignAt
    type: uint64_t

key:
  - Size

TUPLE-TREE-YAML */

#include "revng/ABI/Generated/Early/TypeSpecific.h"

namespace abi {

class TypeSpecific : public generated::TypeSpecific {
public:
  using generated::TypeSpecific::TypeSpecific;
};

} // namespace abi

#include "revng/ABI/Generated/Late/TypeSpecific.h"
