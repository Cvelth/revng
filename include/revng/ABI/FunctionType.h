#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Types.h"

namespace abi::FunctionType {

/// Best effort `CABIFunctionType` to `RawFunctionType` conversion.
///
/// If `ABI` is not specified, `TheBinaryToAddSupportTypesTo.DefaultABI`
/// is used instead.
std::optional<model::CABIFunctionType>
tryConvertToCABI(const model::RawFunctionType &Function,
                 model::Binary &TheBinaryToAddSupportTypesTo,
                 std::optional<model::ABI::Values> ABI = std::nullopt);

/// Best effort `RawFunctionType` to `CABIFunctionType` conversion.
///
/// \note: this convertion is loosy since there's no way to represent some types
/// in `RawFunctionType` in a reversible manner.
std::optional<model::RawFunctionType>
tryConvertToRaw(const model::CABIFunctionType &Function,
                model::Binary &TheBinaryToAddSupportTypesTo);

} // namespace abi::FunctionType
