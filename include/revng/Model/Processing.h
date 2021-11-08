#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <set>

#include "revng/Model/Binary.h"

namespace model {

/// Given \p Types, drop all the types and DynamicFunctions depending on it
///
/// Sometimes you createe a set of placeholder types in the model, but they end
/// up being invalid. In that case, they, and all the types depending on them,
/// need to be dropped.
///
/// \return the number of dropped types
unsigned dropTypesDependingOnTypes(TupleTree<model::Binary> &Binary,
                                   const std::set<const model::Type *> &Types);

/// Given an invalid model which features conflicing names, changes it to make
/// it valid
void deduplicateNames(TupleTree<model::Binary> &Model);

/// Best effort deduplication of types that are identical
void deduplicateEquivalentTypes(TupleTree<model::Binary> &Model);

namespace impl {

//
// Default "failing" upgrade template specialization.
//

template<typename OriginalType, typename TargetType>
Type *tryUpgrade(OriginalType *, TargetType *, Binary &) {
  return nullptr;
}

//
// Explicitly list all the possible upgrades to be specialized elsewhere.
// \todo: typedefs need a bit of extra love, I'll omit them for now.
//

template<>
Type *tryUpgrade(PrimitiveType *, PrimitiveType *, Binary &);
template<>
Type *tryUpgrade(PrimitiveType *, EnumType *, Binary &);
template<>
Type *tryUpgrade(EnumType *, EnumType *, Binary &);
template<>
Type *tryUpgrade(StructType *, StructType *, Binary &);
template<>
Type *tryUpgrade(UnionType *, UnionType *, Binary &);

using CFT = CABIFunctionType;
using RFT = RawFunctionType;
template<>
Type *tryUpgrade(CFT *, CFT *, Binary &);
template<>
Type *tryUpgrade(RFT *, RFT *, Binary &);
template<>
Type *tryUpgrade(CFT *, RFT *, Binary &);

} // namespace impl

/// Best effort type upgrade.
/// It fails (returns a `nullptr`) in cases where types are incompatible or
/// the upgrade would mean rewriting some important characteristic of the
/// `Original` type.
inline Type *tryUpgradingType(Type *Original, Type *Target, Binary &Binary) {
  auto ExternalLambda = [&Target, &Binary](auto &&UpcastedOriginal) {
    auto InternalLambda = [&UpcastedOriginal, &Binary](auto &&UpcastedTarget) {
      using OriginalType = std::decay_t<decltype(UpcastedOriginal)>;
      using TargetType = std::decay_t<decltype(UpcastedTarget)>;
      return impl::tryUpgrade<OriginalType, TargetType>(&UpcastedOriginal,
                                                        &UpcastedTarget,
                                                        Binary);
    };
    return upcast(Target, InternalLambda, static_cast<Type *>(nullptr));
  };
  return upcast(Original, ExternalLambda, static_cast<Type *>(nullptr));
}

} // namespace model
