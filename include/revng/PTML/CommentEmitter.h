#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/PTML/Emitter.h"

namespace ptml {

struct CommentIndicatorPair {
  llvm::StringRef Opening;
  llvm::StringRef Closing;
};

struct CommentIndicators {
  llvm::StringRef Line;
  std::optional<llvm::StringRef> Doxygen = std::nullopt;
  std::optional<CommentIndicatorPair> Block = std::nullopt;
};

class CommentEmitter {
protected:
  ptml::Emitter &PTML;
  const CommentIndicators Indicators;
  const uint64_t WrapAt;

public:
  explicit CommentEmitter(ptml::Emitter &PTML,
                          CommentIndicators Indicators,
                          uint64_t WrapAt) :
    PTML(PTML), Indicators(std::move(Indicators)), WrapAt(WrapAt) {}

public:
  void emitLineComment(llvm::StringRef Content);
  void emitDoxygenComment(llvm::StringRef Content);
  void emitBlockComment(llvm::StringRef Content);

  void emitCategoryComment(llvm::StringRef Content) {
    emitLineComment("");
    emitLineComment(Content);
    emitLineComment("");
    PTML.emitContentNewline();
  }

protected:
  struct Identifier {
    llvm::StringRef Name;
    std::function<void()> CustomEmitter;
  };
  void emitDoxygenLineImpl(llvm::StringRef Keyword,
                           llvm::StringRef Content,
                           std::optional<Identifier> Identifier = std::nullopt);
};

} // namespace ptml
