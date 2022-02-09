#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <variant>

#include "llvm/ADT/SmallVector.h"

#include "revng/Model/Register.h"
#include "revng/Model/Type.h"

/// Describes a return value location.
struct ReturnValueDescription {
public:
  inline size_t getSize() const {
    size_t Result = 0;
    for (model::Register::Values CurrentRegister : Registers)
      Result += model::Register::getSize(CurrentRegister);
    return Result;
  }

public:
  llvm::SmallVector<model::Register::Values, 2> Registers;
};

/// Describes an argument location.
struct ArgumentDescription : public ReturnValueDescription {
public:
  inline size_t getSize() const {
    size_t Result = ReturnValueDescription::getSize();
    if (Stack.has_value())
      Result += Stack->Size;
    return Result;
  }

public:
  struct StackSpan {
    size_t Offset;
    size_t Size;
  };
  std::optional<StackSpan> Stack;
};

/// Describes a function in a reversible manner, can be constructed from either
/// a `model::RawFunctionType` or a `model::CABIFunctionType`.
struct DescribedFunction {
public:
  /// Tries to update the original function based on the changes to this object.
  void tryPropagateChanges() {
    if (FunctionReference.isValid())
      tryPropagateChangesImpl();
  }

  inline ~DescribedFunction() { tryPropagateChanges(); }

private:
  friend std::optional<DescribedFunction>
  describeFunction(model::TypePath Function,
                   model::Binary &TheModel,
                   std::optional<model::ABI::Values> ABI);

  DescribedFunction(model::TypePath FunctionReference,
                    model::Binary &TheModel,
                    model::ABI::Values ABI) :
    ABI(ABI), FunctionReference(FunctionReference), TheModel(TheModel) {}

  void tryPropagateChangesImpl();

public:
  /// Describes the arguments of the function.
  llvm::SmallVector<ArgumentDescription, 4> Arguments;

  /// Describes the return value of the function.
  ReturnValueDescription ReturnValue;

  /// The ABI of the described function.
  const model::ABI::Values ABI;

  /// The `TupleReference` to the described function.
  model::TypePath FunctionReference;

  /// Holds the described function and all the types it refers to.
  model::Binary &TheModel;
};

/// Describes a function type in a reversible manner.
///
/// ABI predecence rules:
/// 1. If the function has `model::CABIFunctionType`, use it's `ABI` member.
/// 2. If \param ABI is not `std::nullopt` or `model::ABI::Invalid`, use it.
/// 3. Otherwise, use `TheModel::DefaultABI`.
///
/// \param Function MUST be either `model::CABIFunctionType` or
/// `model::RawFunctionType`
///
/// \param Function MUST belong to \param TheModel.
std::optional<DescribedFunction>
describeFunction(model::TypePath Function,
                 model::Binary &TheModel,
                 std::optional<model::ABI::Values> ABI = std::nullopt);
