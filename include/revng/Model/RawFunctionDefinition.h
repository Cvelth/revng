#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Identifier.h"
#include "revng/Model/NamedTypedRegister.h"
#include "revng/Model/TypeDefinition.h"

/* TUPLE-TREE-YAML
name: RawFunctionDefinition
type: struct
inherits: TypeDefinition
fields:
  - name: Arguments
    sequence:
      type: SortedVector
      elementType: NamedTypedRegister
  - name: ReturnValues
    sequence:
      type: SortedVector
      elementType: NamedTypedRegister
  - name: ReturnValueComment
    type: string
    optional: true
  - name: PreservedRegisters
    sequence:
      type: SortedVector
      elementType: Register
  - name: FinalStackOffset
    type: uint64_t
  - name: StackArgumentsType
    doc: The type of the struct representing stack arguments
    type: Type
    optional: true
    upcastable: true
TUPLE-TREE-YAML */

#include "revng/Model/Generated/Early/RawFunctionDefinition.h"

class model::RawFunctionDefinition
  : public model::generated::RawFunctionDefinition {
public:
  using generated::RawFunctionDefinition::RawFunctionDefinition;

public:
  /// The helper for stack argument type unwrapping.
  /// Use this when you need to access/modify the existing struct,
  /// and \ref StackArgumentsType() when you need to assign a new one.
  model::StructDefinition *stackArgumentsType() {
    if (StackArgumentsType().empty())
      return nullptr;
    else
      return &StackArgumentsType()->asStruct();
  }

  /// The helper for stack argument type unwrapping.
  /// Use this when you need to access/modify the existing struct,
  /// and \ref StackArgumentsType() when you need to assign a new one.
  const model::StructDefinition *stackArgumentsType() const {
    if (StackArgumentsType().empty())
      return nullptr;
    else
      return &StackArgumentsType()->asStruct();
  }

public:
  const llvm::SmallVector<const model::Type *, 4> edges() const {
    llvm::SmallVector<const model::Type *, 4> Result;

    for (auto &Argument : Arguments())
      if (!Argument.Type().empty())
        Result.push_back(Argument.Type().get());

    for (auto &ReturnValue : ReturnValues())
      if (!ReturnValue.Type().empty())
        Result.push_back(ReturnValue.Type().get());

    if (!StackArgumentsType().empty())
      Result.push_back(StackArgumentsType().get());

    return Result;
  }
};

#include "revng/Model/Generated/Late/RawFunctionDefinition.h"
