#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Type.h"

#include "revng/Pipes/Location/Binary.h"
#include "revng/Pipes/Location/Location.h"

/* TUPLE-TREE-YAML

name: Type
type: struct
fields:
  - name: Key
    type: model::Type::Key

TUPLE-TREE-YAML */

#include "revng/Pipes/Location/Generated/Early/Type.h"

namespace revng::pipes::location::detail {

class Type : public generated::Type {
public:
  using generated::Type::Type;
};

} // namespace revng::pipes::location::detail

#include "revng/Pipes/Location/Generated/Late/Type.h"

namespace revng::pipes::location {

using Type = detail::Location<detail::Type, location::Binary>;

} // namespace revng::pipes::location
