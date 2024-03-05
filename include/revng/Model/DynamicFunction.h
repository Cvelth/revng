#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/MutableSet.h"
#include "revng/Model/Identifier.h"
#include "revng/Model/VerifyHelper.h"

/* TUPLE-TREE-YAML
name: DynamicFunction
doc: Function defined in a dynamic library
type: struct
fields:
  - name: CustomName
    doc: An optional custom name
    type: Identifier
    optional: true
  - name: OriginalName
    doc: The name of the symbol for this dynamic function
    type: string
  - name: Comment
    type: string
    optional: true
  - name: Prototype
    doc: The prototype of the function
    reference:
      pointeeType: TypeDefinition
      rootType: Binary
    optional: true
  - name: Attributes
    doc: Function attributes
    sequence:
      type: MutableSet
      elementType: FunctionAttribute
    optional: true
  - name: Relocations
    sequence:
      type: SortedVector
      elementType: Relocation
    optional: true

key:
  - OriginalName
TUPLE-TREE-YAML */

#include "revng/Model/Generated/Early/DynamicFunction.h"

class model::DynamicFunction : public model::generated::DynamicFunction {
public:
  using generated::DynamicFunction::DynamicFunction;

public:
  Identifier name() const;

  model::DefinitionReference prototype(const model::Binary &Root) const;

public:
  bool verify(bool Assert = false) const debug_function;
  bool verify(VerifyHelper &VH) const;
};

#include "revng/Model/Generated/Late/DynamicFunction.h"
