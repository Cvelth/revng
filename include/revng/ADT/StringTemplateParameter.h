#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <string_view>

/// Allows passing strings as template parameters
///
/// Objects of this type should only ever be constructed from a string literal.
/// Internally, it stores the string (minus the last '\0' character as an array)
template<size_t N>
struct StringTemplateParameter {
  /// \note: it has to be public in order to be used as a template parameter :{
  std::array<char, N - 1> Internal;

  constexpr StringTemplateParameter(const char (&Value)[N]) noexcept {
    std::copy(Value, Value + N - 1, Internal.data());
  }

  constexpr StringTemplateParameter(const StringTemplateParameter &) = default;
  constexpr StringTemplateParameter(StringTemplateParameter &&) = default;
  constexpr StringTemplateParameter &
  operator=(const StringTemplateParameter &) = default;
  constexpr StringTemplateParameter &
  operator=(StringTemplateParameter &&) = default;

  constexpr size_t size() const noexcept { return Internal.size(); }
  constexpr const char *begin() const noexcept { return Internal.begin(); }
  constexpr const char *end() const noexcept { return Internal.end(); }
  constexpr char operator[](size_t Idx) const noexcept { return Internal[Idx]; }

  constexpr operator std::string_view() const noexcept {
    return std::string_view{ Internal.data(), Internal.size() };
  }
};

namespace examples {

template<StringTemplateParameter String>
struct StringParametrizedTrait {
  static constexpr std::string_view value = String;
};

static_assert(StringParametrizedTrait<"value">::value == "value");

} // namespace examples
