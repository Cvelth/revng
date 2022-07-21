#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Type.h"

#include "revng/Pipes/Location/Type.h"
#include "revng/Pipes/Location/Location.h"

/* TUPLE-TREE-YAML

name: TypeField
type: struct
fields:
  - name: TypeKey
    type: model::Type::Key
  - name: Offset
    type: uint64_t

TUPLE-TREE-YAML */

#include "revng/Pipes/Location/Generated/Early/TypeField.h"

namespace revng::pipes::location::detail {

class TypeField : public generated::TypeField {
public:
  using generated::TypeField::TypeField;
};

} // namespace revng::pipes::location::detail

#include "revng/Pipes/Location/Generated/Late/TypeField.h"

namespace revng::pipes::location {

using TypeField = detail::Location<detail::TypeField, location::Type>;

} // namespace revng::pipes::location
