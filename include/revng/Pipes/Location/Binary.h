#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <tuple>

namespace revng::pipes::location {

struct Binary {
  using LocationTag = Binary;
  using Tuple = std::tuple<>;
  using ParentLocation = void;
};

} // namespace revng::pipes::location
