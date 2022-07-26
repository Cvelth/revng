#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/YAMLTraits.h"

#include "revng/ADT/STLExtras.h"
#include "revng/Support/MetaAddress.h"
#include "revng/Support/YAMLTraits.h"

namespace pipeline {

namespace concepts {

template<typename Type>
concept Rank = std::is_same_v<typename Type::RankTag, Type>;

template<typename Type>
concept RootRank = Rank<Type> && Type::Depth == 0;

namespace detail {

template<typename From, typename To>
struct RechabilityHelper {
private:
  static constexpr bool Found = std::is_same_v<From, To>;
  static_assert(Rank<From> && Rank<To>);
  using Next = std::conditional_t<(RootRank<From> || From::Depth <= To::Depth),
                                  std::false_type,
                                  RechabilityHelper<typename From::Parent, To>>;

public:
  static constexpr bool value = Found || Next::value;
};

} // namespace detail

// clang-format off
template<typename First, typename Second>
concept ConvertibleRank = Rank<First>
                          && Rank<Second>
                          && detail::RechabilityHelper<Second, First>::value;
// clang-format on

} // namespace concepts

template<concepts::Rank RankDefinition>
class Location : public RankDefinition::Tuple {
  friend RankDefinition;

private:
  using Tuple = typename RankDefinition::Tuple;
  static_assert(std::tuple_size_v<Tuple> == RankDefinition::Depth);
  static constexpr std::size_t Size = RankDefinition::Depth;

private:
  constexpr static std::string_view Separator = "/";

public:
  using Tuple::Tuple;
  Location(const Location &) = default;
  Location(Location &&) = default;
  Location &operator=(const Location &) = default;
  Location &operator=(Location &&) = default;

  Tuple &tuple() { return *this; }
  const Tuple &tuple() const { return *this; }

  template<concepts::ConvertibleRank<RankDefinition> AnotherRank>
  auto &get(const AnotherRank &Rank) {
    return std::get<AnotherRank::Depth - 1>(tuple());
  }
  template<concepts::ConvertibleRank<RankDefinition> AnotherRank>
  const auto &get(const AnotherRank &Rank) const {
    return std::get<AnotherRank::Depth - 1>(tuple());
  }

  // clang-format off
  template<typename AnotherRank>
    requires (concepts::ConvertibleRank<RankDefinition, AnotherRank>
              || concepts::ConvertibleRank<AnotherRank, RankDefinition>)
  static constexpr Location<RankDefinition>
  convert(Location<AnotherRank> &&Another) {
    // clang-format on
    Location<RankDefinition> Result;

    constexpr auto Common = std::min(RankDefinition::Depth, AnotherRank::Depth);
    constexprRepeat<Common>([&Result, &Another]<std::size_t I> {
      std::get<I>(Result.tuple()) = std::move(std::get<I>(Another.tuple()));
    });

    return Result;
  }

  // clang-format off
  template<typename AnotherRank>
    requires (concepts::ConvertibleRank<RankDefinition, AnotherRank>
              || concepts::ConvertibleRank<AnotherRank, RankDefinition>)
  static constexpr Location<RankDefinition>
  convert(const Location<AnotherRank> &Another) {
    // clang-format on
    Location<RankDefinition> Result;

    constexpr auto Common = std::min(RankDefinition::Depth, AnotherRank::Depth);
    constexprRepeat<Common>([&Result, &Another]<std::size_t I> {
      std::get<I>(Result.tuple()) = std::get<I>(Another.tuple());
    });

    return Result;
  }

public:
  std::string toString() const {
    std::string Result;

    (Result += Separator) += RankDefinition::RankName;
    constexprRepeat<Size>([&Result, this]<auto Index> {
      (Result += Separator) += serializeToString(std::get<Index>(tuple()));
    });

    return Result;
  }

  static constexpr std::optional<Location<RankDefinition>>
  fromString(std::string_view String) {
    Location<RankDefinition> Result;

    auto MaybeSteps = constexprSplit<Size + 2>(Separator, String);
    if (!MaybeSteps.has_value())
      return std::nullopt;

    revng_assert(MaybeSteps.value().size() == Size + 2);
    constexpr std::string_view ExpectedName = RankDefinition::RankName;
    if (MaybeSteps->at(0) != "" || MaybeSteps->at(1) != ExpectedName)
      return std::nullopt;

    auto Success = constexprAnd<Size>([&Result, &MaybeSteps]<auto Index> {
      using T = typename std::tuple_element<Index, Tuple>::type;
      auto MaybeValue = detail::deserializeImpl<T>(MaybeSteps->at(Index + 2));
      if (!MaybeValue)
        return false;

      std::get<Index>(Result) = std::move(*MaybeValue);
      return true;
    });
    if (Success == false)
      return std::nullopt;

    return Result;
  }
};

} // namespace pipeline

template<pipeline::concepts::Rank Rank>
struct llvm::yaml::ScalarTraits<pipeline::Location<Rank>> {
  static void output(const pipeline::Location<Rank> &Value,
                     void *,
                     llvm::raw_ostream &Output) {
    Output << Value.toString();
  }

  static StringRef
  input(llvm::StringRef Scalar, void *, pipeline::Location<Rank> &Value) {
    auto MaybeValue = pipeline::Location<Rank>::fromString(Scalar);
    revng_assert(MaybeValue.has_value());
    Value = std::move(*MaybeValue);
    return StringRef();
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::Double; }
};
