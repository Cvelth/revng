#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipes/Location/Binary.h"
#include "revng/Pipes/Location/Location.h"

/* TUPLE-TREE-YAML

name: Function
type: struct
fields:
  - name: Entry
    type: MetaAddress

TUPLE-TREE-YAML */

#include "revng/Pipes/Location/Generated/Early/Function.h"

namespace revng::pipes::location::detail {

class Function : public generated::Function {
public:
  using generated::Function::Function;
};

} // namespace revng::pipes::location::detail

#include "revng/Pipes/Location/Generated/Late/Function.h"

namespace revng::pipes::location {

using Function = detail::Location<detail::Function, location::Binary>;

} // namespace revng::pipes::location
