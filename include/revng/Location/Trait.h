#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Location/Kind.h"
#include "revng/Model/Type.h"
#include "revng/Support/MetaAddress.h"

namespace revng::location {

template<Kind::Values>
struct Trait;

template<>
struct Trait<Kind::Binary> {
  constexpr static size_t Rank = 0;
  constexpr static Kind::Values Parent = Kind::Invalid;

  using type = void;
};

template<>
struct Trait<Kind::Function> {
  constexpr static size_t Rank = 1;
  constexpr static Kind::Values Parent = Kind::Binary;

  using type = MetaAddress;
};

template<>
struct Trait<Kind::BasicBlock> {
  constexpr static size_t Rank = 2;
  constexpr static Kind::Values Parent = Kind::Function;

  using type = MetaAddress;
};

template<>
struct Trait<Kind::Instruction> {
  constexpr static size_t Rank = 3;
  constexpr static Kind::Values Parent = Kind::BasicBlock;

  using type = MetaAddress;
};

template<>
struct Trait<Kind::Type> {
  constexpr static size_t Rank = 1;
  constexpr static Kind::Values Parent = Kind::Binary;

  using type = model::Type::Key;
};

template<>
struct Trait<Kind::TypeField> {
  constexpr static size_t Rank = 2;
  constexpr static Kind::Values Parent = Kind::Type;

  using type = uint64_t; // Not sure about how this one should look like.
};

template<>
struct Trait<Kind::RawByte> {
  constexpr static size_t Rank = 1;
  constexpr static Kind::Values Parent = Kind::Binary;

  using type = MetaAddress;
};

template<>
struct Trait<Kind::RawByteRange> {
  constexpr static size_t Rank = 2;
  constexpr static Kind::Values Parent = Kind::RawByte;

  using type = MetaAddress;
};

} // namespace revng::location
