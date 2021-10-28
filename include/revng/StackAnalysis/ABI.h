#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/Type.h"
#include "revng/StackAnalysis/ABI/CallingConventionTrait.h"

namespace abi {

// using RegisterState = model::RegisterState::Values;
// using RegisterStateMap = std::map<model::Register::Values,
//                                   std::pair<RegisterState, RegisterState>>;

template<model::abi::Values V>
class ABI {
  static_assert(V != model::abi::Invalid);
  static_assert(V != model::abi::Count);

public:
  using CallingConvention = CallingConventionTrait<V>;

  static std::optional<model::CABIFunctionType>
  toCABI(model::Binary &TheBinary, const model::RawFunctionType &Explicit);
  static std::optional<model::RawFunctionType>
  toRaw(model::Binary &TheBinary, const model::CABIFunctionType &Original);

  static model::TypePath defaultPrototype(model::Binary &TheBinary);

  // static bool isCompatible(const model::RawFunctionType &Explicit);
  // void applyDeductions(RegisterStateMap &Prototype);
};

std::optional<model::CABIFunctionType>
convertToCABI(model::abi::Values,
              model::Binary &,
              const model::RawFunctionType &);
std::optional<model::RawFunctionType>
convertToRaw(model::abi::Values,
             model::Binary &,
             const model::CABIFunctionType &);
model::TypePath defaultPrototype(model::abi::Values, model::Binary &);

inline std::optional<model::RawFunctionType>
convertToRaw(model::Binary &TheBinary, const model::Type *T) {
  revng_assert(T != nullptr);

  using namespace llvm;
  if (auto *Raw = dyn_cast<model::RawFunctionType>(T)) {
    return *Raw;
  } else if (auto *CABI = dyn_cast<model::CABIFunctionType>(T)) {
    return convertToRaw(CABI->ABI, TheBinary, *CABI);
  } else {
    revng_abort("getRawFunctionType with non-function type");
  }
}

inline model::RawFunctionType
convertToRawOrDefault(model::Binary &TheBinary, const model::Type *T) {
  using RFT = model::RawFunctionType;
  if (auto MaybeResult = convertToRaw(TheBinary, T))
    return *MaybeResult;
  else if (auto *CABI = llvm::dyn_cast<model::CABIFunctionType>(T))
    return *llvm::cast<RFT>(defaultPrototype(CABI->ABI, TheBinary).get());
  else
    revng_abort("getRawFunctionType with non-function type");
}

} // namespace abi
