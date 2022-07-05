#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Location/Trait.h"
#include "revng/Support/MetaAddress/YAMLTraits.h"

namespace revng::location {

namespace detail {

struct Empty {};

template<revng::location::Kind::Values Kind>
struct Impl : public std::conditional_t<(Trait<Kind>::Rank > 1),
                                        Impl<Trait<Kind>::Parent>,
                                        Empty> {
public:
  constexpr static bool HasParent = Trait<Kind>::Rank > 1;
  using MaybeParent = Impl<Trait<Kind>::Parent>;
  using Parent = std::conditional_t<HasParent, MaybeParent, Empty>;

  constexpr static bool NeedsValue = Trait<Kind>::Rank > 0;
  using MaybeValueType = typename Trait<Kind>::type;
  using ValueType = std::conditional_t<NeedsValue, MaybeValueType, Empty>;

  // We need a virtual destructor if we want to support non-trivial types.
  static_assert(std::is_trivially_destructible_v<ValueType>);

public:
  ValueType Value;

protected:
  // clang-format off
  template<revng::location::Kind::Values LocalKind = Kind>
    requires (Trait<LocalKind>::Rank == 0 || Yamlizable<ValueType>)
  std::string toStringImpl() const {
    // clang-format on

    if constexpr (Trait<LocalKind>::Rank == 0)
      return "";
    else if constexpr (Trait<LocalKind>::Rank == 1)
      return "/" + serializeToString(Value);
    else
      return Impl<Trait<LocalKind>::Parent>::toStringImpl() + "/"
             + serializeToString(Value);
  }

  // clang-format off
  template<revng::location::Kind::Values LocalKind = Kind>
    requires (Trait<Kind>::Rank == 0 || Yamlizable<ValueType>)
  static std::optional<Impl<LocalKind>>
  fromStringImpl(llvm::ArrayRef<llvm::StringRef> Input) {
    // clang-format on

    revng_assert(Input.size() == Trait<LocalKind>::Rank);

    Impl<LocalKind> Result;
    if constexpr (Trait<LocalKind>::Rank > 1) {
      using PrevLevel = Impl<Trait<LocalKind>::Parent>;
      if (auto MaybeResult = PrevLevel::fromStringImpl(Input.drop_back())) {
        Parent &Reference = Result;
        Reference = std::move(*MaybeResult);
      } else {
        return std::nullopt;
      }
    }

    if constexpr (Trait<LocalKind>::Rank != 0) {
      using Deserialized = typename Trait<LocalKind>::type;
      auto MaybeValue = ::detail::deserializeImpl<Deserialized>(Input.back());
      if (MaybeValue)
        Result.Value = *MaybeValue;
      else
        return std::nullopt;
    }

    return Result;
  }

public:
  std::string toString() const {
    return "/" + Kind::getName(Kind).str() + toStringImpl();
  };

  static std::optional<Impl<Kind>> fromString(llvm::StringRef Input) {
    revng_assert(!Input.empty());
    revng_assert(Input[0] == '/');
    Input = Input.drop_front();

    llvm::SmallVector<llvm::StringRef, 4> Parsed;
    size_t CurrentPosition = Input.find_first_of('/');
    while (CurrentPosition != std::string_view::npos) {
      Parsed.emplace_back(Input.take_front(CurrentPosition));
      Input = Input.drop_front(CurrentPosition + 1);

      CurrentPosition = Input.find_first_of('/');
    }

    if (!Input.empty())
      Parsed.emplace_back(std::move(Input));

    revng_assert(Parsed.size() > 0);

    auto ParsedKind = location::Kind::fromName(Parsed[0]);
    if (ParsedKind != Kind)
      return std::nullopt;

    return fromStringImpl({ std::next(Parsed.begin()), Parsed.end() });
  }
};

} // namespace detail

using Binary = detail::Impl<Kind::Binary>;

using Function = detail::Impl<Kind::Function>;
using BasicBlock = detail::Impl<Kind::BasicBlock>;
using Instruction = detail::Impl<Kind::Instruction>;

using Type = detail::Impl<Kind::Type>;
using TypeField = detail::Impl<Kind::TypeField>;

using RawByte = detail::Impl<Kind::RawByte>;
using RawByteRange = detail::Impl<Kind::RawByteRange>;

// Convenience aliases
using FromByte = RawByte;
using ToByte = RawByteRange;

} // namespace revng::location

template<revng::location::Kind::Values Kind>
struct llvm::yaml::ScalarTraits<revng::location::detail::Impl<Kind>> {
  using Impl = revng::location::detail::Impl<Kind>;

  static void output(const Impl &Value, void *, llvm::raw_ostream &Output) {
    Output << Value.toString();
  }

  static StringRef input(llvm::StringRef Scalar, void *, Impl &Value) {
    Value = Impl::fromString(Scalar);
    return StringRef();
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::Double; }
};
