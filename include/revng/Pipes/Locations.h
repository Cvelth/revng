#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Type.h"
#include "revng/Pipeline/ArtifactLocationKind.h"
#include "revng/Support/MetaAddress.h"

namespace revng::pipes::location {

namespace detail {

template<StringTemplateParameter... NameValues>
using N = ArtifactLocationNameHelper<NameValues...>;

template<typename Names = N<>, typename... Types>
using Kind = pipeline::ArtifactLocationKind<Names, Types...>;

using Key = model::Type::Key;

} // namespace detail

inline detail::Kind<> Binary("binary");

inline detail::Kind<detail::N<"entry">, MetaAddress>
  Function("function", Binary);
inline detail::Kind<detail::N<"function", "start">, MetaAddress, MetaAddress>
  BasicBlock("basic-block", Function);
inline detail::Kind<detail::N<"function", "basic-block", "address">,
                    MetaAddress,
                    MetaAddress,
                    MetaAddress>
  Instruction("instruction", BasicBlock);

inline detail::Kind<detail::N<"key">, model::Type::Key> Type("type", Binary);
inline detail::Kind<detail::N<"type-key", "field-index">, detail::Key, uint64_t>
  TypeField("type-field", Type);

inline detail::Kind<detail::N<"address">, MetaAddress>
  RawByte("raw-byte", Binary);
inline detail::Kind<detail::N<"from", "to">, MetaAddress, MetaAddress>
  RawByteRange("raw-byte-range", RawByte);

} // namespace revng::pipes::location
