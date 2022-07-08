#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <optional>

#include "revng/Support/ArtifactLocation.h"
#include "revng/Support/Assert.h"
#include "revng/Support/DynamicHierarchy.h"

namespace pipeline {

struct ArtifactLocationKindBase
  : public DynamicHierarchy<ArtifactLocationKindBase> {

  using DynamicHierarchy<ArtifactLocationKindBase>::DynamicHierarchy;
};

template<typename Names, typename... Types>
class ArtifactLocationKind : public ArtifactLocationKindBase {
public:
  ArtifactLocationKind(llvm::StringRef Name) : ArtifactLocationKindBase(Name) {}

  ArtifactLocationKind(llvm::StringRef Name, ArtifactLocationKindBase &Parent) :
    ArtifactLocationKindBase(Name, Parent) {}

  virtual ~ArtifactLocationKind() = default;

public:
  using Instance = ArtifactLocation<Names, Types...>;

  // clang-format off
  template<typename... OtherTypes>
    requires(sizeof...(OtherTypes) == sizeof...(Types) &&
             sizeof...(OtherTypes) != 0 &&
             (std::is_constructible_v<Types, OtherTypes> && ...))
  Instance make(OtherTypes... Values) {
    // clang-format on
    Instance Result{ std::forward<OtherTypes>(Values)... };
    Result.Kind = name();
    return Result;
  }

  Instance make() {
    Instance Result;
    Result.Kind = name();
    return Result;
  }
  std::optional<Instance> fromString(llvm::StringRef Input) {
    revng_assert(!Input.empty());
    revng_assert(Input[0] == '/');
    Input = Input.drop_front();

    llvm::SmallVector<llvm::StringRef, 4> Parsed;
    size_t CurrentPosition = Input.find_first_of('/');
    while (CurrentPosition != llvm::StringRef::npos) {
      Parsed.emplace_back(Input.take_front(CurrentPosition));
      Input = Input.drop_front(CurrentPosition + 1);

      CurrentPosition = Input.find_first_of('/');
    }

    if (!Input.empty())
      Parsed.emplace_back(std::move(Input));

    revng_assert(Parsed.size() > 0);
    if (Parsed[0] != name())
      return std::nullopt;

    Instance Res;
    Res.Kind = name();
    if constexpr (sizeof...(Types) == 0) {
      return Res;
    } else if (Res.fromStringImpl({ std::next(Parsed.begin()), Parsed.end() }))
      return Res;
    else
      return std::nullopt;
  }
};

} // namespace pipeline
