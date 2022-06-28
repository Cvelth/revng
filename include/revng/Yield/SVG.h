#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <string>

class MetaAddress;
namespace model {
class Binary;
}
namespace yield {
class CallGraph;
class Function;
} // namespace yield

namespace yield {

namespace svg {

std::string controlFlow(const yield::Function &InternalFunction,
                        const model::Binary &Binary);

std::string calls(const yield::CallGraph &Graph, const model::Binary &Binary);

std::string callsSlice(const MetaAddress &SlicePoint,
                       const yield::CallGraph &Graph,
                       const model::Binary &Binary);

} // namespace svg

} // namespace yield
