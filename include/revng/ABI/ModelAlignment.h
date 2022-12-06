#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ABI/Definition.h"
#include "revng/ADT/RecursiveCoroutine.h"
#include "revng/Model/Type.h"
#include "revng/Model/VerifyHelper.h"

namespace abi {

RecursiveCoroutine<std::optional<uint64_t>>
naturalAlignment(model::VerifyHelper &VH,
                 const model::QualifiedType &Type,
                 const abi::Definition &ABI);

/// \brief Compute the natural alignment of the type as expected by the selected
///        ABI when the type is passed as a parameter.
///
/// \note  This function belong to the `revngModel` library, but due to
///        a circular dependency, it's placed here for now.
///
/// \note  It mirrors, `model::Type::trySize()` pretty closely, see
///        documentation related to it (and usage of the coroutines inside
///        this codebase in general) for more details on how it works.
///
/// \param Type The type to compute the alignment of.
/// \param ABI The ABI used to determine alignment of the primitive components
///        of the type
///
/// \return either an alignment or a `std::nullopt` when it's not applicable.
inline std::optional<uint64_t>
naturalAlignment(const model::QualifiedType &Type, const abi::Definition &ABI) {
  model::VerifyHelper VH;
  return naturalAlignment(VH, Type, ABI);
}

} // namespace abi
