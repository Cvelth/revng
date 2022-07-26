#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/StringTemplateParameter.h"
#include "revng/Pipeline/Location.h"
#include "revng/Support/DynamicHierarchy.h"

namespace pipeline {

/// The rank tree is a tree used by targets to find out how many names
/// are required to name a target
class Rank : public DynamicHierarchy<Rank> {
public:
  Rank(llvm::StringRef Name) : DynamicHierarchy(Name) {}
  Rank(llvm::StringRef Name, Rank &Parent) : DynamicHierarchy(Name, Parent) {}
};

namespace detail {

template<typename... TypesToAppend, typename... InitialTupleTypes>
inline constexpr auto appendImpl(std::tuple<InitialTupleTypes...> &&Tuple) {
  return std::tuple<InitialTupleTypes..., TypesToAppend...>();
}

template<typename Tuple, typename... TypesToAppend>
struct AppendToTupleHelper {
  using type = decltype(appendImpl<TypesToAppend...>(std::declval<Tuple>()));
};

template<typename First, typename Second>
struct CH {
  template<std::size_t Index>
  constexpr bool operator()() {
    return std::is_convertible_v<std::tuple_element_t<Index, First>,
                                 std::tuple_element_t<Index, Second>>;
  }
};
template<typename Tuple>
inline constexpr auto TS = std::tuple_size_v<Tuple>;

// clang-format off
template<typename LHS, typename RHS>
concept TupleConvertibilityCheck = TS<LHS> == TS<RHS> &&
                                   constexprAnd<TS<LHS>>(CH<LHS, RHS>{});
// clang-format on

} // namespace detail

template<StringTemplateParameter Str, typename LocationType, typename ParentRnk>
class RankDefinition : public Rank {
public:
  static constexpr std::string_view RankName = Str;
  using Type = LocationType;
  using Parent = ParentRnk;

  using Definition = RankDefinition<Str, Type, Parent>;
  using RankTag = Definition;

public:
  static_assert(std::is_same_v<Parent, typename Parent::Definition>);
  static constexpr size_t Depth = Parent::Depth + 1;
  using Tuple = typename detail::AppendToTupleHelper<typename Parent::Tuple,
                                                     Type>::type;

public:
  explicit RankDefinition(Parent &ParentObj) : Rank(RankName, ParentObj) {}

  // clang-format off
  template<typename ...Args>
    requires detail::TupleConvertibilityCheck<std::tuple<Args...>, Tuple>
  constexpr static Location<Definition> makeLocation(Args &&...Arguments) {
    // clang-format on
    return Location<Definition>(Arguments...);
  }

  constexpr static std::optional<Location<Definition>>
  locationFromString(std::string_view String) {
    return Location<Definition>::fromString(String);
  }
};

// Root rank specialization
template<StringTemplateParameter Str>
class RankDefinition<Str, void, void> : public Rank {
public:
  static constexpr std::string_view RankName = Str;
  using Type = void;
  using Parent = void;

  using Definition = RankDefinition<Str, Type, Parent>;
  using RankTag = Definition;

public:
  static constexpr size_t Depth = 0;
  using Tuple = std::tuple<>;

public:
  explicit RankDefinition() : Rank(RankName) {}

  constexpr static Location<Definition> makeLocation() {
    return Location<Definition>{};
  }

  constexpr static std::optional<Location<Definition>>
  locationFromString(std::string_view String) {
    return Location<Definition>::fromString(String);
  }
};
template<StringTemplateParameter Str>
using RootRankDefinition = RankDefinition<Str, void, void>;

// Rank template parameter deduction helper.
template<StringTemplateParameter Name, typename Type, typename Parent>
pipeline::RankDefinition<Name, Type, Parent> defineRank(Parent &ParentObject) {
  return pipeline::RankDefinition<Name, Type, Parent>(ParentObject);
}

} // namespace pipeline
