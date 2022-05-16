#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <limits>
#include <string>

#include "revng/ADT/SortedVector.h"
#include "revng/Model/VerifyHelper.h"
#include "revng/Support/MetaAddress.h"
#include "revng/Support/MetaAddress/YAMLTraits.h"
#include "revng/Yield/Internal/BasicBlockType.h"
#include "revng/Yield/Internal/Instruction.h"

/* TUPLE-TREE-YAML

name: BasicBlock
type: struct
fields:
  - name: Address
    type: MetaAddress
  - name: NextAddress
    type: MetaAddress
  - name: Type
    type: yield::BasicBlockType::Values

  - name: Instructions
    sequence:
      type: SortedVector
      elementType: yield::Instruction
  - name: Calls
    sequence:
      type: SortedVector
      elementType: MetaAddress
    optional: true
  - name: Targets
    sequence:
      type: SortedVector
      elementType: MetaAddress
    optional: true

  - name: CommentIndicator
    type: std::string
  - name: LabelIndicator
    type: std::string

key:
  - Address

TUPLE-TREE-YAML */

#include "revng/Yield/Internal/Generated/Early/BasicBlock.h"

namespace yield {

class BasicBlock : public generated::BasicBlock {
public:
  using generated::BasicBlock::BasicBlock;

public:
  bool verify(model::VerifyHelper &VH) const;
  void dump() const debug_function;

public:
  inline bool verify() const debug_function { return verify(false); }
  inline bool verify(bool Assert) const debug_function {
    model::VerifyHelper VH(Assert);
    return verify(VH);
  }
};

} // namespace yield

#include "revng/Yield/Internal/Generated/Late/BasicBlock.h"
