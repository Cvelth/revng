#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <iterator>
#include <optional>
#include <set>
#include <string_view>
#include <type_traits>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/iterator_range.h"

#include <array>
#include <concepts>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>

/// Allows passing strings as template parameters
///
/// Objects of this type should only ever be constructed from a string literal.
/// Internally, it stores the string (minus the last '\0' character as an array)
template<size_t N = 1>
struct ConstexprString {
public:
  /// The data members have to be public for a literal type to be usable as
  /// a template parameter :{
  std::array<char, N - 1> String;

  constexpr ConstexprString(const char (&Value)[N]) noexcept
    requires(N >= 2)
  {
    std::copy(Value, Value + N - 1, String.data());
  }

  constexpr ConstexprString() noexcept
    requires(N == 1)
    : String({}) {}

  constexpr ConstexprString(const ConstexprString &) = default;
  constexpr ConstexprString(ConstexprString &&) = default;
  constexpr ConstexprString &operator=(const ConstexprString &) = default;
  constexpr ConstexprString &operator=(ConstexprString &&) = default;

  constexpr size_t size() const noexcept { return String.size(); }
  constexpr const char *begin() const noexcept { return String.begin(); }
  constexpr const char *end() const noexcept { return String.end(); }
  constexpr char operator[](size_t Idx) const noexcept { return String[Idx]; }

  constexpr operator std::string_view() const noexcept {
    return std::string_view{ String.data(), String.size() };
  }
};

namespace examples {

template<ConstexprString String>
struct StringParametrizedTrait {
  static constexpr std::string_view value = String;
};

static_assert(StringParametrizedTrait<"value">::value == "value");

} // namespace examples

//
// The concepts from the STL.
// TODO: remove these after updating the libc++ version.
//

template<typename T>
concept equality_comparable = requires(T &&LHS, T &&RHS) {
  { LHS == RHS } -> std::convertible_to<bool>;
};

// clang-format off
template<typename F, typename... Args>
concept invocable = requires(F&& Function, Args &&...Arguments) {
  std::invoke(std::forward<F>(Function), std::forward<Args>(Arguments)...);
};

// NOTE: this is supposed to use `std::regular_invocable`, but the difference
//       is not major in most cases, so use `invocable` until we update libc++.
template<typename F, typename... Args>
concept predicate = invocable<F, Args...>
                    && std::is_convertible_v<std::invoke_result_t<F, Args...>,
                                             bool>;
// clang-format on

/// TODO: Remove after updating to clang-format with concept support.
struct ClangFormatPleaseDoNotBreakMyCode;
// clang-format off
// clang-format on

//
// Concepts to simplify working with tuples.
//

template<class T>
concept TupleSizeCompatible = requires {
  std::tuple_size<T>::value;
  { std::tuple_size_v<T> } -> std::convertible_to<size_t>;
};

static_assert(TupleSizeCompatible<std::tuple<>>);
static_assert(!TupleSizeCompatible<std::vector<int>>);
static_assert(!TupleSizeCompatible<int>);

namespace revng::detail {

template<class T, std::size_t N>
concept TupleElementCompatibleHelper = requires(T Value) {
  typename std::tuple_element_t<N, std::remove_const_t<T>>;
  { get<N>(Value) } -> std::convertible_to<std::tuple_element_t<N, T> &>;
};

template<typename T, size_t... N>
constexpr auto checkTupleElementTypes(std::index_sequence<N...>) {
  return (TupleElementCompatibleHelper<T, N> && ...);
}

template<TupleSizeCompatible T>
constexpr auto checkAllTupleElementTypes() {
  constexpr std::size_t Size = std::tuple_size_v<T>;
  return checkTupleElementTypes<T>(std::make_index_sequence<Size>());
}

} // namespace revng::detail

// clang-format off
template<class T>
concept TupleLike = (TupleSizeCompatible<T>
                     and revng::detail::checkAllTupleElementTypes<T>());
// clang-format on

static_assert(TupleLike<std::tuple<>>);
static_assert(TupleLike<std::tuple<int, int, long>>);
static_assert(TupleLike<std::pair<int, int>>);
static_assert(TupleLike<std::array<int, 0>>);
static_assert(not TupleLike<std::vector<int>>);

//
// Concepts to simplify working with specializations of templates.
//

/// A concept that helps determine whether a given object is a specialization
/// (or is inheriting a specialization) of a given template.
///
/// This lets us cut down on the number of `IsX` (e.g. `IsGenericGraph`,
/// `IsRank`) concepts needed since we can now just say
/// `SpecializationOf<GenericGraph>` on the interface boundaries and get
/// the expected check.
template<typename Type, template<typename...> class Ref>
concept SpecializationOf = requires(Type &Value) {
  []<typename... Ts>(Ref<Ts...> &) {
  }(const_cast<std::remove_const_t<Type> &>(Value));
};

static_assert(SpecializationOf<std::pair<int, long>, std::pair>);
static_assert(SpecializationOf<const std::pair<int, long>, std::pair>);
static_assert(SpecializationOf<std::string, std::basic_string>);
static_assert(SpecializationOf<std::ostream, std::basic_ios>);
static_assert(not SpecializationOf<std::string, std::basic_string_view>);

/// TODO: Remove after updating to clang-format with concept support.
struct ClangFormatPleaseDoNotBreakMyCode;
// clang-format off
// clang-format on

namespace revng::detail {

template<typename Test, template<typename...> class Ref>
struct StrictSpecializationHelper : std::false_type {};

template<template<typename...> class Ref, typename... Args>
struct StrictSpecializationHelper<Ref<Args...>, Ref> : std::true_type {};

template<template<typename...> class Ref, typename... Args>
struct StrictSpecializationHelper<const Ref<Args...>, Ref> : std::true_type {};

template<typename Test, template<typename...> class Ref>
constexpr bool
  StrictSpecialization = StrictSpecializationHelper<Test, Ref>::value;

} // namespace revng::detail

/// A more strict version of the `SpecializationOf` concept. This one only
/// allows direct specializations (and aliases) - no inheritance.
///
/// It's useful in the cases when template parameter deduction is important,
/// e.g. when instantiating traits.
template<typename Test, template<typename...> class Ref>
concept StrictSpecializationOf = revng::detail::StrictSpecialization<Test, Ref>;

static_assert(StrictSpecializationOf<std::pair<int, long>, std::pair>);
static_assert(StrictSpecializationOf<const std::pair<int, long>, std::pair>);
static_assert(StrictSpecializationOf<std::string, std::basic_string>);
static_assert(not StrictSpecializationOf<std::ostream, std::basic_ios>);
static_assert(not StrictSpecializationOf<std::string, std::basic_string_view>);

//
// Other Miscellaneous concepts.
//

template<typename T, typename R>
concept ConstOrNot = std::is_same_v<R, T> or std::is_same_v<const R, T>;

// clang-format off
template<class Range, typename ValueType>
concept range_with_value_type = std::ranges::range<Range> &&
                std::is_convertible_v<typename Range::value_type,
                                      ValueType>;
// clang-format on

//
// always_true and always_false
//
// Since an assert in the `else` branch of an `if_constexpr` condition said
// branch gets instantiated if it doesn't depend on a template, these provide
// an easy way to "fake" dependence on an arbitrary template parameter.
//

template<typename T>
struct type_always_false {
  constexpr static bool value = false;
};
template<typename T>
constexpr inline bool type_always_false_v = type_always_false<T>::value;

template<auto V>
struct value_always_false {
  constexpr static bool value = false;
};
template<auto V>
constexpr inline bool value_always_false_v = value_always_false<V>::value;

template<typename T>
struct type_always_true {
  constexpr static bool value = false;
};
template<typename T>
constexpr inline bool type_always_true_v = type_always_true<T>::value;

template<auto V>
struct value_always_true {
  constexpr static bool value = false;
};
template<auto V>
constexpr inline bool value_always_true_v = value_always_true<V>::value;

//===----------------------------------------------------------------------===//
//     Extra additions to <iterator>
//===----------------------------------------------------------------------===//

namespace revng {
namespace detail {

template<typename FuncTy, typename ItTy>
using ReturnType = decltype(std::declval<FuncTy>()(*std::declval<ItTy>()));

template<typename ItTy,
         typename FuncTy,
         typename FuncReturnTy = ReturnType<FuncTy, ItTy>>
class ProxyMappedIteratorImpl : public llvm::mapped_iterator<ItTy, FuncTy> {
  struct IteratorProxy {
    IteratorProxy(FuncReturnTy &&Value) : Temporary(std::move(Value)) {}
    FuncReturnTy *const operator->() { return &Temporary; }
    FuncReturnTy const *const operator->() const { return &Temporary; }

  private:
    FuncReturnTy Temporary;
  };

public:
  using llvm::mapped_iterator<ItTy, FuncTy>::mapped_iterator;
  using reference = std::decay_t<FuncReturnTy>;

  IteratorProxy operator->() {
    return llvm::mapped_iterator<ItTy, FuncTy>::operator*();
  }
  IteratorProxy const operator->() const {
    return llvm::mapped_iterator<ItTy, FuncTy>::operator*();
  }
};

template<typename ItTy, typename FuncTy>
using ItImpl = std::conditional_t<std::is_object_v<ReturnType<FuncTy, ItTy>>,
                                  ProxyMappedIteratorImpl<ItTy, FuncTy>,
                                  llvm::mapped_iterator<ItTy, FuncTy>>;

} // namespace detail

/// `revng::mapped_iterator` is a specialized version of
/// `llvm::mapped_iterator`.
///
/// It can act as an in-place replacement since it doesn't change the behavior
/// in most cases. The main difference is the fact that when the iterator uses
/// a temporary as a way of remembering its position its lifetime is
/// explicitly prolonged to prevent it from being deleted prematurely (like
/// inside the `operator->` call).
template<typename ItTy, typename FuncTy>
using mapped_iterator = revng::detail::ItImpl<ItTy, FuncTy>;

// `map_iterator` - Provide a convenient way to create `mapped_iterator`s,
// just like `make_pair` is useful for creating pairs...
template<class ItTy, class FuncTy>
inline auto map_iterator(ItTy I, FuncTy F) {
  return mapped_iterator<ItTy, FuncTy>(std::move(I), std::move(F));
};

template<class ContainerTy, class FuncTy>
auto map_range(ContainerTy &&C, FuncTy F) {
  return llvm::make_range(map_iterator(C.begin(), F), map_iterator(C.end(), F));
}

auto dereferenceIterator(auto Iter) {
  return llvm::map_iterator(Iter, [](const auto &Ptr) -> decltype(*Ptr) & {
    return *Ptr;
  });
}

namespace detail {
template<typename T>
using DIT = decltype(dereferenceIterator(std::declval<T>()));
}

template<typename T>
using DereferenceIteratorType = revng::detail::DIT<T>;

template<typename T>
using DereferenceRangeType = llvm::iterator_range<revng::detail::DIT<T>>;

auto dereferenceRange(auto &&Range) {
  return llvm::make_range(dereferenceIterator(Range.begin()),
                          dereferenceIterator(Range.end()));
}

template<typename Iterator>
auto mapToValueIterator(Iterator It) {
  const auto GetSecond = [](auto &Pair) -> auto & { return Pair.second; };
  return llvm::map_iterator(It, GetSecond);
}

template<typename T>
using MapToValueIteratorType = decltype(mapToValueIterator(std::declval<T>()));

} // namespace revng

//
// skip
//

namespace revng::detail {

// Remove these incomplete iterator testers after we update to libc++-13+
// with standard library concept support.
// NOTE: they are VERY basic, don't rely on them too much.

template<typename Iterator>
using Category = typename std::iterator_traits<Iterator>::iterator_category;

template<typename Iterator>
concept InputOnly = std::is_same_v<Category<Iterator>, std::input_iterator_tag>;
template<typename Iterator>
concept OutputOnly = std::is_same_v<Category<Iterator>,
                                    std::output_iterator_tag>;
template<typename Iterator>
concept ForwardOnly = std::is_same_v<Category<Iterator>,
                                     std::forward_iterator_tag>;
template<typename Iterator>
concept BidirectionalOnly = std::is_same_v<Category<Iterator>,
                                           std::bidirectional_iterator_tag>;
template<typename Iterator>
concept RandomAccessOnly = std::is_same_v<Category<Iterator>,
                                          std::random_access_iterator_tag>;
template<typename Iterator>
concept ContiguousOnly = std::is_same_v<Category<Iterator>,
                                        std::contiguous_iterator_tag>;

// clang-format off
template<typename Iterator>
concept contiguous_iterator = ContiguousOnly<Iterator>;
template<typename Iterator>
concept random_access_iterator = contiguous_iterator<Iterator>
                                 || RandomAccessOnly<Iterator>;
template<typename Iterator>
concept bidirectional_iterator = random_access_iterator<Iterator>
                                 || BidirectionalOnly<Iterator>;
template<typename Iterator>
concept forward_iterator = bidirectional_iterator<Iterator>
                           || ForwardOnly<Iterator>;
template<typename Iterator>
concept input_iterator = forward_iterator<Iterator> || InputOnly<Iterator>;
template<typename Iterator>
concept input_or_output_iterator = input_iterator<Iterator>
                                   || OutputOnly<Iterator>;
// clang-format on

template<typename IteratorType>
inline auto
skipImpl(IteratorType &&From,
         IteratorType &&To,
         std::size_t Front = 0,
         std::size_t Back = 0) -> llvm::iterator_range<IteratorType> {

  std::ptrdiff_t TotalSkippedCount = Front + Back;
  if constexpr (forward_iterator<IteratorType>) {
    // We cannot compute the assert on the input iterators because it's
    // going to consume them.
    revng_assert(std::distance(From, To) >= TotalSkippedCount);
  }

  std::decay_t<IteratorType> Begin{ From };
  std::advance(Begin, Front);

  std::decay_t<IteratorType> End{ To };
  std::advance(End, -(std::ptrdiff_t) Back);

  return llvm::make_range(std::move(Begin), std::move(End));
}

template<bidirectional_iterator T>
inline decltype(auto)
skip(T &&From, T &&To, std::size_t Front = 0, std::size_t Back = 0) {
  return skipImpl(std::forward<T>(From), std::forward<T>(To), Front, Back);
}

template<input_iterator T>
inline decltype(auto) // NOLINTNEXTLINE
skip_front(T &&From, T &&To, std::size_t SkippedCount = 1) {
  return skipImpl(std::forward<T>(From), std::forward<T>(To), SkippedCount, 0);
}

template<bidirectional_iterator T>
inline decltype(auto) // NOLINTNEXTLINE
skip_back(T &&From, T &&To, std::size_t SkippedCount = 1) {
  return skipImpl(std::forward<T>(From), std::forward<T>(To), 0, SkippedCount);
}

} // namespace revng::detail

template<std::ranges::range T>
inline decltype(auto)
skip(T &&Range, std::size_t Front = 0, std::size_t Back = 0) {
  return revng::detail::skip(Range.begin(), Range.end(), Front, Back);
}

template<std::ranges::range T> // NOLINTNEXTLINE
inline decltype(auto) skip_front(T &&Range, std::size_t SkippedCount = 1) {
  return revng::detail::skip_front(Range.begin(), Range.end(), SkippedCount);
}

template<std::ranges::range T> // NOLINTNEXTLINE
inline decltype(auto) skip_back(T &&Range, std::size_t SkippedCount = 1) {
  return revng::detail::skip_back(Range.begin(), Range.end(), SkippedCount);
}

//
// slice
//

/// Copy into a std::array a slice of an llvm::ArrayRef
template<size_t Start, size_t Size, typename T>
std::array<T, Size> slice(llvm::ArrayRef<T> Old) {
  std::array<T, Size> Result;
  auto StartIt = Old.begin() + Start;
  std::copy(StartIt, StartIt + Size, Result.begin());
  return Result;
}

/// Copy into a std::array a slice of a std::array
template<size_t Start, size_t Size, typename T, size_t OldSize>
std::array<T, Size> slice(const std::array<T, OldSize> &Old) {
  std::array<T, Size> Result;
  auto StartIt = Old.begin() + Start;
  std::copy(StartIt, StartIt + Size, Result.begin());
  return Result;
}

/// Simple helper function asserting a pointer is not a `nullptr`
template<typename T>
inline T *notNull(T *Pointer) {
  revng_assert(Pointer != nullptr);
  return Pointer;
}

inline llvm::ArrayRef<uint8_t> toArrayRef(llvm::StringRef Data) {
  auto Pointer = reinterpret_cast<const uint8_t *>(Data.data());
  return llvm::ArrayRef<uint8_t>(Pointer, Data.size());
}

//
// append
//
template<std::ranges::sized_range FromType, std::ranges::sized_range ToType>
auto append(FromType &&From, ToType &To) {
  size_t ExistingElementCount = To.size();
  To.resize(ExistingElementCount + From.size());
  return llvm::copy(From, std::next(To.begin(), ExistingElementCount));
}

/// Intersects two std::sets
template<typename T>
std::set<T *> intersect(const std::set<T *> &First, const std::set<T *> &Last) {
  std::set<T *> Output;
  std::set_intersection(First.begin(),
                        First.end(),
                        Last.begin(),
                        Last.end(),
                        std::inserter(Output, Output.begin()));
  return Output;
}

//
// constexpr repeat
//

namespace detail {

template<typename TemplatedCallableType, std::size_t... Indices>
constexpr void constexprRepeatImpl(std::index_sequence<Indices...>,
                                   TemplatedCallableType &&Callable) {
  (Callable.template operator()<Indices>(), ...);
}

template<typename TemplatedCallableType, std::size_t... Indices>
constexpr bool constexprAndImpl(std::index_sequence<Indices...>,
                                TemplatedCallableType &&Callable) {
  return (Callable.template operator()<Indices>() && ...);
}

template<typename TemplatedCallableType, std::size_t... Indices>
constexpr bool constexprOrImpl(std::index_sequence<Indices...>,
                               TemplatedCallableType &&Callable) {
  return (Callable.template operator()<Indices>() || ...);
}

} // namespace detail

template<std::size_t IterationCount, typename CallableType>
constexpr void constexprRepeat(CallableType &&Callable) {
  detail::constexprRepeatImpl(std::make_index_sequence<IterationCount>(),
                              std::forward<CallableType>(Callable));
}

template<std::size_t IterationCount, typename CallableType>
constexpr bool constexprAnd(CallableType &&Callable) {
  return detail::constexprAndImpl(std::make_index_sequence<IterationCount>(),
                                  std::forward<CallableType>(Callable));
}

template<std::size_t IterationCount, typename CallableType>
constexpr bool constexprOr(CallableType &&Callable) {
  return detail::constexprOrImpl(std::make_index_sequence<IterationCount>(),
                                 std::forward<CallableType>(Callable));
}

namespace examples {
using namespace std::string_view_literals;

template<std::size_t Count>
consteval std::size_t fullSize(std::array<std::string_view, Count> Components,
                               std::string_view Separator) {
  std::size_t Result = Separator.size() * Count;
  constexprRepeat<Count>([&Result, &Components]<std::size_t Index> {
    Result += std::get<Index>(Components).size();
  });
  return Result;
}

inline constexpr std::array Components = { "instruction"sv,
                                           "0x401000:Code_x86_64"sv,
                                           "0x402000:Code_x86_64"sv,
                                           "0x403000:Code_x86_64"sv };
static_assert(fullSize(Components, "/"sv) == 75);

} // namespace examples

//
// constexpr split
//

namespace detail {

template<std::size_t N, std::size_t I = 0>
inline constexpr bool
constexprSplitHelper(std::array<std::string_view, N> &Result,
                     std::string_view Separator,
                     std::string_view Input) {
  std::size_t Position = Input.find(Separator);
  if constexpr (I < N - 1) {
    if (Position == std::string_view::npos)
      return false;

    Result[I] = Input.substr(0, Position);
    return constexprSplitHelper<N, I + 1>(Result,
                                          Separator,
                                          Input.substr(Position + 1));
  } else {
    if (Position != std::string_view::npos)
      return false;

    Result[I] = Input;
    return true;
  }
}

} // namespace detail

/// I'm forced to implement my own split because `llvm::StringRef`'s alternative
/// is not `constexpr`-compatible.
///
/// This also uses `std::string_view` instead of `llvm::StringRef` because its
/// `find` member is constexpr - hence at least that member doesn't have to be
/// reimplemented
template<std::size_t N>
inline constexpr std::optional<std::array<std::string_view, N>>
constexprSplit(std::string_view Separator, std::string_view Input) {
  if (std::array<std::string_view, N> Result;
      detail::constexprSplitHelper<N>(Result, Separator, Input))
    return Result;
  else
    return std::nullopt;
}

inline void
replaceAll(std::string &Input, const std::string &From, const std::string &To) {
  if (From.empty())
    return;

  size_t Start = 0;
  while ((Start = Input.find(From, Start)) != std::string::npos) {
    Input.replace(Start, From.length(), To);
    Start += To.length();
  }
}

//
// `constexpr` versions of the llvm algorithm adaptors.
//

namespace revng {

/// \note use `llvm::find` instead after it's made `constexpr`.
template<typename R, typename T>
constexpr decltype(auto) find(R &&Range, const T &Value) {
  return std::find(std::begin(std::forward<R>(Range)),
                   std::end(std::forward<R>(Range)),
                   Value);
}

/// \note use `llvm::find_if` instead after it's made `constexpr`.
template<typename R, typename CallableType> // NOLINTNEXTLINE
constexpr decltype(auto) find_if(R &&Range, CallableType &&Callable) {
  return std::find_if(std::begin(std::forward<R>(Range)),
                      std::end(std::forward<R>(Range)),
                      std::forward<CallableType>(Callable));
}

/// \note use `llvm::find_if_not` instead after it's made `constexpr`.
template<typename R, typename CallableType> // NOLINTNEXTLINE
constexpr decltype(auto) find_if_not(R &&Range, CallableType &&Callable) {
  return std::find_if_not(std::begin(std::forward<R>(Range)),
                          std::end(std::forward<R>(Range)),
                          std::forward<CallableType>(Callable));
}

/// \note `std::find_last` is introduced in c++23,
///       replace with the llvm version when it's available.
template<typename R, typename T> // NOLINTNEXTLINE
constexpr decltype(auto) find_last(R &&Range, const T &Value) {
  return std::find(std::rbegin(std::forward<R>(Range)),
                   std::rend(std::forward<R>(Range)),
                   Value);
}

/// \note `std::find_last_if` is introduced in c++23,
///       replace with the llvm version when it's available.
template<typename R, typename CallableType> // NOLINTNEXTLINE
constexpr decltype(auto) find_last_if(R &&Range, CallableType &&Callable) {
  return std::find_if(std::rbegin(std::forward<R>(Range)),
                      std::rend(std::forward<R>(Range)),
                      std::forward<CallableType>(Callable));
}

/// \note `std::find_last_if_not` is introduced in c++23,
///       replace with the llvm version when it's available.
template<typename R, typename CallableType> // NOLINTNEXTLINE
constexpr decltype(auto) find_last_if_not(R &&Range, CallableType &&Callable) {
  return std::find_if_not(std::rbegin(std::forward<R>(Range)),
                          std::rend(std::forward<R>(Range)),
                          std::forward<CallableType>(Callable));
}

/// \note use `llvm::is_contained` instead after it's made `constexpr`.
template<typename R, typename T> // NOLINTNEXTLINE
constexpr bool is_contained(R &&Range, const T &Value) {
  return revng::find(std::forward<R>(Range), Value) != std::end(Range);
}

template<typename Range, typename C> // NOLINTNEXTLINE
constexpr bool is_contained_if(Range &&R, C &&L) {
  return find_if(std::forward<Range>(R), std::forward<C>(L)) != std::end(R);
}

static_assert(is_contained(std::array{ 1, 2, 3 }, 2) == true);
static_assert(is_contained(std::array{ 1, 2, 3 }, 4) == false);

} // namespace revng

//
// Some views from the STL.
// TODO: remove these after updating the libc++ version.
//
template<typename RangeType> // NOLINTNEXTLINE
auto as_rvalue(RangeType &&Range) {
  return llvm::make_range(std::make_move_iterator(Range.begin()),
                          std::make_move_iterator(Range.end()));
}
