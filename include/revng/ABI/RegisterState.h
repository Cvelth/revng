#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include "revng/Model/Architecture.h"
#include "revng/Model/Register.h"
#include "revng/Support/Assert.h"

namespace abi::RegisterState {

// WIP: drop?
class Map {
public:
  struct StatePair {
    bool IsUsedForPassingArguments = false;
    bool IsUsedForReturningValues = false;
  };

public:
  explicit Map(model::Architecture::Values Architecture) :
    Architecture(Architecture),
    Internal(model::Register::getCount(Architecture)) {}

  StatePair &at(model::Register::Values Requested) {
    return Internal[Requested - model::Register::getFirst(Architecture)];
  }
  const StatePair &at(model::Register::Values Requested) const {
    return Internal[Requested - model::Register::getFirst(Architecture)];
  }

  StatePair &operator[](model::Register::Values Requested) {
    return at(Requested);
  }
  const StatePair &operator[](model::Register::Values Requested) const {
    return at(Requested);
  }

  size_t size() const { return Internal.size(); }

private:
  std::pair<model::Register::Values, StatePair &> mapIt(StatePair &Value) {
    auto Offset = to_underlying(model::Register::getFirst(Architecture));
    Offset += std::distance(&Internal.front(), &Value);
    return { model::Register::Values(Offset), Value };
  }
  std::pair<model::Register::Values, const StatePair &>
  mapIt(const StatePair &Value) const {
    auto Offset = to_underlying(model::Register::getFirst(Architecture));
    Offset += std::distance(&Internal.front(), &Value);
    return { model::Register::Values(Offset), Value };
  }

  auto mapped_range() {
    auto L = [this](StatePair &Value) { return this->mapIt(Value); };
    return llvm::make_range(revng::map_iterator(Internal.begin(), L),
                            revng::map_iterator(Internal.end(), L));
  }
  auto reverse_mapped_range() {
    auto L = [this](StatePair &Value) { return this->mapIt(Value); };
    return llvm::make_range(revng::map_iterator(Internal.rbegin(), L),
                            revng::map_iterator(Internal.rend(), L));
  }
  auto const_mapped_range() const {
    auto L = [this](const StatePair &Value) { return this->mapIt(Value); };
    return llvm::make_range(revng::map_iterator(Internal.begin(), L),
                            revng::map_iterator(Internal.end(), L));
  }
  auto const_reverse_mapped_range() const {
    auto L = [this](const StatePair &Value) { return this->mapIt(Value); };
    return llvm::make_range(revng::map_iterator(Internal.rbegin(), L),
                            revng::map_iterator(Internal.rend(), L));
  }

public:
  auto begin() { return mapped_range().begin(); }
  auto rbegin() { return reverse_mapped_range().begin(); }
  auto cbegin() const { return const_mapped_range().begin(); }
  auto crbegin() const { return const_reverse_mapped_range().begin(); }

  auto end() { return mapped_range().end(); }
  auto rend() { return reverse_mapped_range().end(); }
  auto cend() const { return const_mapped_range().end(); }
  auto crend() const { return const_reverse_mapped_range().end(); }

  auto begin() const { return cbegin(); }
  auto end() const { return cend(); }

private:
  /// \note: this field is dangerous to mutate - keep it private
  model::Architecture::Values Architecture;
  llvm::SmallVector<StatePair, 64> Internal;
};

} // namespace abi::RegisterState
