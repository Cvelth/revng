#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/SmallVector.h"

#include "revng/ABI/Predefined.h"
#include "revng/ADT/STLExtras.h"
#include "revng/Model/ABI.h"
#include "revng/Model/Register.h"

namespace abi {

template<ranges::sized_range Container>
llvm::SmallVector<model::Register::Values, 8>
orderArguments(const Container &Registers, const abi::Definition &ABI) {
  SortedVector<model::Register::Values> Lookup;
  {
    auto Inserter = Lookup.batch_insert();
    for (auto &&Register : Registers)
      Inserter.insert(Register);
  }

  llvm::SmallVector<model::Register::Values, 8> Result;
  for (auto Register : ABI.GeneralPurposeArgumentRegisters)
    if (Lookup.count(Register) != 0)
      Result.emplace_back(Register);
  for (auto Register : ABI.VectorArgumentRegisters)
    if (Lookup.count(Register) != 0)
      Result.emplace_back(Register);

  revng_assert(Result.size() == std::size(Registers));
  return Result;
}

template<ranges::sized_range Container>
llvm::SmallVector<model::Register::Values, 8>
orderReturnValues(const Container &Registers, const abi::Definition &ABI) {
  SortedVector<model::Register::Values> Lookup;
  {
    auto Inserter = Lookup.batch_insert();
    for (auto &&Register : Registers)
      Inserter.insert(Register);
  }

  llvm::SmallVector<model::Register::Values, 8> Result;
  for (auto Register : ABI.GeneralPurposeReturnValueRegisters)
    if (Lookup.count(Register) != 0)
      Result.emplace_back(Register);
  for (auto Register : ABI.VectorReturnValueRegisters)
    if (Lookup.count(Register) != 0)
      Result.emplace_back(Register);

  revng_assert(Result.size() == std::size(Registers));
  return Result;
}

} // namespace abi
