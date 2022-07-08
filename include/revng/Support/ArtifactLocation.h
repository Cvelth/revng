#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/KeyedObjectTraits.h"
#include "revng/ADT/StringTemplateParameter.h"
#include "revng/Support/YAMLTraits.h"

template<StringTemplateParameter... Names>
struct ArtifactLocationNameHelper {
  using ContainerType = std::array<std::string_view, sizeof...(Names)>;
  static constexpr ContainerType Value{ Names... };
  static constexpr size_t Count = sizeof...(Names);

public:
  template<StringTemplateParameter Name>
  constexpr static std::optional<size_t> find() {
    return findImpl<Name>();
  }
  template<StringTemplateParameter Name>
  constexpr static bool contains() {
    return find<Name>() != std::nullopt;
  }

private:
  template<StringTemplateParameter Name, size_t CurrentIndex = 0>
  constexpr static std::optional<size_t> findImpl() {
    if constexpr (CurrentIndex >= Count)
      return std::nullopt;
    else if constexpr (Name == Value[CurrentIndex])
      return CurrentIndex;
    else
      return findImpl<Name, CurrentIndex + 1>();
  }
};

namespace pipeline {

template<typename Names = ArtifactLocationNameHelper<>, typename... Types>
class ArtifactLocationKind;

} // namespace pipeline

template<typename Names, typename... Types>
struct ArtifactLocation : public std::tuple<Types...> {
  using std::tuple<Types...>::tuple;
  using NameHelper = Names;
  friend pipeline::ArtifactLocationKind<Names, Types...>;

  template<size_t Index>
  using ElementType = std::tuple_element_t<Index, std::tuple<Types...>>;

private:
  std::string_view Kind;

public:
  std::string toString() const {
    std::string Result = '/' + std::string(Kind);
    if constexpr (sizeof...(Types) > 0)
      Result += toStringImpl();
    return Result;
  }

private:
  template<size_t Layer = 0>
  std::string toStringImpl() const {
    std::string Result = '/' + serializeToString(std::get<Layer>(*this));
    if constexpr (Layer < sizeof...(Types) - 1)
      Result += toStringImpl<Layer + 1>();
    return Result;
  }

  // clang-format off
  template<size_t Layer = 0>
  ArtifactLocation *fromStringImpl(llvm::ArrayRef<llvm::StringRef> Input) {
    // clang-format on

    static_assert(Layer < sizeof...(Types));
    revng_assert(Input.size() == sizeof...(Types));

    using T = typename std::tuple_element_t<Layer, std::tuple<Types...>>;
    if (auto MaybeValue = ::detail::deserializeImpl<T>(Input[Layer]))
      std::get<Layer>(*this) = *MaybeValue;
    else
      return nullptr;

    if constexpr (Layer < sizeof...(Types) - 1)
      return fromStringImpl<Layer + 1>(Input);
    else
      return this;
  }
};

namespace detail {

template<typename Names, typename... Types>
using L = ArtifactLocation<Names, Types...>;

} // namespace detail

namespace std {

// clang-format off

template<StringTemplateParameter Name, typename Helper, typename... Types>
  requires(Helper::template find<Name>().has_value())
typename std::tuple_element<Helper::template find<Name>().value(),
                            std::tuple<Types...>>::type &
get(detail::L<Helper, Types...> &Obj) noexcept {
  static constexpr auto I = Helper::template find<Name>().value();
  return std::get<I>(static_cast<detail::L<Helper, Types...> &>(Obj));
}

template<StringTemplateParameter Name, typename Helper, typename... Types>
  requires(Helper::template find<Name>().has_value())
typename std::tuple_element<Helper::template find<Name>().value(),
                            std::tuple<Types...>>::type &&
get(detail::L<Helper, Types...> &&Obj) noexcept {
  static constexpr auto I = Helper::template find<Name>().value();
  return std::get<I>(static_cast<detail::L<Helper, Types...> &&>(Obj));
}

template<StringTemplateParameter Name, typename Helper, typename... Types>
  requires(Helper::template find<Name>().has_value())
typename std::tuple_element<Helper::template find<Name>().value(),
                            std::tuple<Types...>>::type const &
get(const detail::L<Helper, Types...> &Obj) noexcept {
  static constexpr auto I = Helper::template find<Name>().value();
  return std::get<I>(static_cast<const detail::L<Helper, Types...> &>(Obj));
}

template<StringTemplateParameter Name, typename Helper, typename... Types>
  requires(Helper::template find<Name>().has_value())
typename std::tuple_element<Helper::template find<Name>().value(),
                            std::tuple<Types...>>::type const &&
get(const detail::L<Helper, Types...> &&Obj) noexcept {
  static constexpr auto I = Helper::template find<Name>().value();
  return std::get<I>(static_cast<const detail::L<Helper, Types...> &&>(Obj));
}

// clang-format on

} // namespace std

template<typename Names, typename... Types>
struct llvm::yaml::ScalarTraits<ArtifactLocation<Names, Types...>> {
  using Location = ArtifactLocation<Names, Types...>;
  static void output(const Location &Value, void *, llvm::raw_ostream &Output) {
    Output << Value.toString();
  }
  static StringRef input(llvm::StringRef Scalar, void *, Location &Value) {
    Value = Location::fromString(Scalar);
    return StringRef();
  }
  static QuotingType mustQuote(StringRef) { return QuotingType::Double; }
};

template<typename Ns, typename... Ts>
struct KeyedObjectTraits<ArtifactLocation<Ns, Ts...>>
  : public IdentityKeyedObjectTraits<ArtifactLocation<Ns, Ts...>> {};
