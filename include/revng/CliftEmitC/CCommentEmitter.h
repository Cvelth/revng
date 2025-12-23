#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Clift/Clift.h"
#include "revng/PTML/CTokenEmitter.h"
#include "revng/PTML/CommentEmitter.h"

namespace mlir::clift {

class CCommentEmitter : public ptml::CommentEmitter {
  ptml::CTokenEmitter &Tokens;

public:
  explicit CCommentEmitter(ptml::CTokenEmitter &Tokens, uint64_t WrapAt) :
    ptml::CommentEmitter(Tokens.comments(WrapAt)), Tokens(Tokens) {}

public:
  void emitFunctionComment(mlir::clift::FunctionOp Function);
};

} // namespace mlir::clift
