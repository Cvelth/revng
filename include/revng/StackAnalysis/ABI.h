#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/Type.h"
#include "revng/StackAnalysis/ABI/AllowedRegisters.h"

namespace abi {

using RegisterState = model::RegisterState::Values;
using RegisterStateMap = std::map<model::Register::Values,
                                  std::pair<RegisterState, RegisterState>>;

template<model::abi::Values V>
class ABI {
private:
  using Allowed = AllowedRegisterList<V>;

public:
  static bool isCompatible(const model::RawFunctionType &Explicit);

  static std::optional<model::CABIFunctionType>
  toCABI(model::Binary &TheBinary, const model::RawFunctionType &Explicit) {
    return {};
  }

  static std::optional<model::RawFunctionType>
  toRaw(model::Binary &TheBinary, const model::CABIFunctionType &Original) {
    return {};
  }

  static model::TypePath defaultPrototype(model::Binary &TheBinary) {
    model::TypePath
      Void = TheBinary.getPrimitiveType(model::PrimitiveTypeKind::Void, 0);
    return TheBinary.recordNewType(model::makeType<model::RawFunctionType>());
  }

  void applyDeductions(RegisterStateMap &Prototype) { return; }
};

// TODO: make this as much reusable as possible
// TODO: test

template<typename T, size_t Index = 0>
auto polyswitch(T Value, const auto &F) {
  constexpr T Current = static_cast<T>(Index);
  if constexpr (Index < static_cast<size_t>(T::Count)) {
    if (Current == Value) {
      return F.template operator()<Current>();
    } else {
      return polyswitch<T, Index + 1>(Value, F);
    }
  } else {
    revng_abort();
    return F.template operator()<T::Count>();
  }
}

inline std::optional<model::RawFunctionType>
getRawFunctionType(model::Binary &TheBinary,
                   const model::CABIFunctionType *CABI) {
  revng_assert(CABI != nullptr);

  return polyswitch(CABI->ABI, [&]<model::abi::Values A>() {
    return ABI<A>::toRaw(TheBinary, *CABI);
  });
}

inline std::optional<model::RawFunctionType>
getRawFunctionType(model::Binary &TheBinary, const model::Type *T) {
  revng_assert(T != nullptr);

  using namespace llvm;
  if (auto *Raw = dyn_cast<model::RawFunctionType>(T)) {
    return *Raw;
  } else if (auto *CABI = dyn_cast<model::CABIFunctionType>(T)) {
    return getRawFunctionType(TheBinary, CABI);
  } else {
    revng_abort("getRawFunctionType with non-function type");
  }
}

inline model::RawFunctionType
getRawFunctionTypeOrDefault(model::Binary &TheBinary, const model::Type *T) {
  revng_assert(T != nullptr);

  using namespace llvm;
  if (auto *Raw = dyn_cast<model::RawFunctionType>(T)) {
    return *Raw;
  } else if (auto *CABI = dyn_cast<model::CABIFunctionType>(T)) {
    auto MaybeResult = getRawFunctionType(TheBinary, CABI);
    if (MaybeResult) {
      return *MaybeResult;
    } else {
      auto GetDefaultPrototype = [&]<model::abi::Values A>() {
        return ABI<A>::defaultPrototype(TheBinary);
      };
      model::TypePath Result = polyswitch(CABI->ABI, GetDefaultPrototype);

      return *cast<model::RawFunctionType>(Result.get());
    }
  } else {
    revng_abort("getRawFunctionType with non-function type");
  }
}

} // namespace abi
