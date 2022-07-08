#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include "revng/ADT/STLExtras.h"
#include "revng/Pipeline/ArtifactLocationKind.h"
#include "revng/Support/Assert.h"

namespace pipeline {

/// A ArtifactLocationRegistry is a simple vector used to keep track of which
/// kinds are available in a particular pipeline
class ArtifactLocationRegistry {
public:
  using Container = llvm::SmallVector<ArtifactLocationKindBase *, 16>;

private:
  Container Kinds;

public:
  ArtifactLocationRegistry(Container Kinds = {}) : Kinds(std::move(Kinds)) {}

  void registerLocation(ArtifactLocationKindBase &K) {
    revng_assert(not contains(K.name()));
    Kinds.push_back(&K);
  }

public:
  auto begin() { return revng::dereferenceIterator(Kinds.begin()); }
  auto end() { return revng::dereferenceIterator(Kinds.end()); }
  auto begin() const { return revng::dereferenceIterator(Kinds.begin()); }
  auto end() const { return revng::dereferenceIterator(Kinds.end()); }

  const ArtifactLocationKindBase *find(llvm::StringRef Name) const {
    auto Iterator = llvm::find_if(Kinds, [Name](const auto *Kind) {
      return Kind->name() == Name;
    });
    if (Iterator == Kinds.end())
      return nullptr;
    return *Iterator;
  }

  bool contains(llvm::StringRef Name) { return find(Name) != nullptr; }

  size_t size() const { return Kinds.size(); }

public:
  template<typename OS>
  void dump(OS &OStream) const {
    for (const auto &K : *this)
      OStream << K.name().str() << "\n";
  }

  void dump() const debug_function { dump(dbg); }
};

} // namespace pipeline
