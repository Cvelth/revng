#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <tuple>

#include "revng/ADT/STLExtras.h"
#include "revng/TupleTree/TupleLikeTraits.h"

namespace revng::pipes::location::detail {

namespace concepts {

template<typename Type>
concept Location = std::is_same_v<typename std::decay_t<Type>::LocationTag,
                                  std::decay_t<Type>>;

// clang-format off
template<typename T>
concept RootLocation = Location<T> &&
                       std::is_same_v<typename std::decay_t<T>::ParentLocation,
                                      void>;
// clang-format on

namespace detail {

template<typename First, typename Second>
struct RechabilityHelper {
private:
  static constexpr bool
    Found = std::is_same<std::decay_t<First>, std::decay_t<Second>>::value;

  template<bool C, typename T, typename F>
  using Cond = std::conditional_t<C, T, F>;
  using DS = std::decay_t<Second>;
  using Next = Cond<RootLocation<DS>,
                    std::false_type,
                    RechabilityHelper<First, typename DS::ParentLocation>>;

public:
  static constexpr bool value = Found || Next::value;
};

template<typename LHS, typename RHS>
struct EquivalencyHelper {
  // clang-format off
  static constexpr bool value = std::is_convertible_v<LHS, RHS> &&
                                std::is_convertible_v<RHS, LHS>;
  // clang-format on
};

template<typename First, typename Second, std::size_t RelevantSize>
struct IterationHelper {
private:
  using FirstTuple = typename std::decay_t<First>::Tuple;
  using SecondTuple = typename std::decay_t<Second>::Tuple;

public:
  static constexpr bool
    value = RechabilityHelper<First, Second>::value
            && constexprAnd<RelevantSize>([]<auto Index>() {
                 using LHS = std::tuple_element_t<Index, FirstTuple>;
                 using RHS = std::tuple_element_t<Index, SecondTuple>;
                 return EquivalencyHelper<LHS, RHS>::value;
               });
};

template<typename First, typename Second>
struct CompatibilityHelper {
private:
  using FirstTuple = typename std::decay_t<First>::Tuple;
  static constexpr auto FirstSize = std::tuple_size<FirstTuple>::value;

  using SecondTuple = typename std::decay_t<Second>::Tuple;
  static constexpr auto SecondSize = std::tuple_size<SecondTuple>::value;

  static constexpr auto Size = std::min(FirstSize, SecondSize);

public:
  static constexpr bool value = (FirstSize <= SecondSize) ?
                                  IterationHelper<First, Second, Size>::value :
                                  IterationHelper<Second, First, Size>::value;
};

} // namespace detail

// clang-format off
template<typename F, typename S>
concept CompatibleLocation = Location<F> && Location<S>
                               && detail::CompatibilityHelper<F, S>::value;
// clang-format on

} // namespace concepts

template<typename Base, typename Parent>
class Location : public Base {
public:
  using Tuple = typename TupleLikeTraits<Base>::tuple;
  using LocationTag = Location<Base, Parent>;
  using ParentLocation = Parent;

private:
  static_assert(concepts::Location<Parent>);
  static_assert(std::tuple_size<Tuple>::value);

  template<std::size_t CommonElementCount, typename TupleType>
  void moveHelper(TupleType &&Tuple) {
    constexprRepeat<CommonElementCount>([&]<std::size_t Index>() {
      get<Index>(tuple()) = get<Index>(std::forward<TupleType>(Tuple));
    });
  }

public:
  using Base::Base;
  Base &tuple() { return *this; }
  const Base &tuple() const { return *this; }
  Base &&moveTuple() { return std::move(*this); }

  template<concepts::CompatibleLocation<Location<Base, Parent>> AnotherLocation>
  explicit Location(AnotherLocation &&Another) {
    using AnotherTuple = typename std::decay_t<AnotherLocation>::Tuple;
    static constexpr auto
      CommonElementCount = std::min(std::tuple_size_v<Tuple>,
                                    std::tuple_size_v<AnotherTuple>);
    if constexpr (std::is_rvalue_reference<AnotherLocation>::value)
      moveHelper<CommonElementCount>(Another.moveTuple());
    else
      moveHelper<CommonElementCount>(Another.tuple());
  }
};

} // namespace revng::pipes::location::detail
