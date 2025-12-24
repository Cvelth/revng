//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <ranges>

#include "llvm/ADT/StringRef.h"

#include "revng/ADT/STLExtras.h"
#include "revng/PTML/CommentEmitter.h"
#include "revng/PTML/Constants.h"
#include "revng/PTML/Emitter.h"

// FUTURE-WIP comment (will remove after the review):
//
// This is a cut-down version of `CommentBuilder` from `Doxygen.cpp`.
// I'm not doing the full rewrite to conserve time, but this is definitely
// worth looking into as a part of the PR that will convert assembly to the new
// emitters.

namespace ptml::tokens {

static constexpr auto Keyword = "doxygen.keyword";

} // namespace ptml::tokens

namespace ptml {

struct Attribute {
  std::string Name;
  std::string Value;
};

struct DoxygenToken {
  enum class Types {
    /// Indicates that no special formatting is needed.
    Untagged,

    /// Wraps spaces together so they can be easier to work with by the UI.
    Indentation,

    /// Indicates that the contained text is a keyword, so it should be
    /// formatted as such.
    Keyword,

    /// Indicates that the contained text is a name, so it should be
    /// formatted as such.
    Identifier
  };

  Types Type;
  std::string Value;

  std::optional<std::function<void()>> CustomEmitter = {};

public:
  /// \note Consumes the value: must only be used once per object.
  void emit(ptml::Emitter &PTML) {
    if (Type == DoxygenToken::Types::Untagged) {
      PTML.emitContent(Value);
      return;
    }

    if (Type == DoxygenToken::Types::Identifier) {
      revng_assert(CustomEmitter.has_value());
      CustomEmitter.value()();
      return;
    }

    llvm::StringRef TokenType = "";
    switch (Type) {
    case DoxygenToken::Types::Indentation:
      TokenType = ptml::tokens::Indentation;
      break;
    case DoxygenToken::Types::Keyword:
      TokenType = ptml::tokens::Keyword;
      break;
    default:
      revng_abort("Unsupported doxygen tag.");
    }

    auto EmittedTag = PTML.initializeOpenTag(ptml::tags::Span);
    EmittedTag.emitAttribute(ptml::attributes::Token, TokenType);
    EmittedTag.finalizeOpenTag();

    PTML.emitContent(Value);
  }
};

struct DoxygenLine {
  llvm::SmallVector<DoxygenToken, 8> Tags;
  size_t InternalIndentation;

  llvm::SmallVector<DoxygenToken, 8> *operator->() { return &Tags; }
};

struct CommentEmitterImpl {
  ptml::Emitter &PTML;
  llvm::StringRef Indicator;
  size_t WrapAt;

public:
  void emit(DoxygenLine &&Line) {
    auto &&[ResultLine, CurrentSize] = line();

    if (Line.Tags.empty())
      emitLine(std::move(ResultLine));

    bool WereTagsEmittedSinceLastBreak = false;
    for (auto Iterator = Line->begin(); Iterator != Line->end(); ++Iterator) {
      emitToken(*Iterator,
                Line.InternalIndentation,
                ResultLine,
                CurrentSize,
                WereTagsEmittedSinceLastBreak);
    }

    if (WereTagsEmittedSinceLastBreak)
      emitLine(std::move(ResultLine));
  }

  void emit(DoxygenToken Token) {
    emit(DoxygenLine{ .Tags = { std::move(Token) }, .InternalIndentation = 0 });
  }

  void emit(llvm::StringRef Text) {
    emit(DoxygenToken{ .Type = DoxygenToken::Types::Untagged,
                       .Value = Text.str() });
  }

private:
  void emitToken(const DoxygenToken &Token,
                 size_t InternalIndentation,
                 DoxygenLine &ResultLine,
                 size_t &CurrentSize,
                 bool &WereTagsEmittedSinceLastBreak) {
    llvm::StringRef TagText = Token.Value;
    while (!TagText.empty()) {
      if (size_t NewLinePosition = TagText.find('\n');
          NewLinePosition != TagText.npos
          && NewLinePosition + CurrentSize < WrapAt) {
        revng_assert(Token.Type == DoxygenToken::Types::Untagged,
                     "Line breaks are only allowed in untagged sections.");

        // There's a new line character in the next tag, use it as
        // the break point
        if (NewLinePosition != 0)
          ResultLine->emplace_back(DoxygenToken::Types::Untagged,
                                   TagText.substr(0, NewLinePosition).str(),
                                   Token.CustomEmitter);

        auto NextCharacter = std::min(NewLinePosition + 1, TagText.size());
        TagText = TagText.drop_front(NextCharacter);

        emitLine(std::move(ResultLine));
        std::tie(ResultLine, CurrentSize) = line(InternalIndentation);
        WereTagsEmittedSinceLastBreak = false;
      } else if (TagText.size() + CurrentSize < WrapAt) {
        // The rest of the tag fits into this line, just append it as is.
        ResultLine->emplace_back(DoxygenToken::Types::Untagged,
                                 TagText.str(),
                                 Token.CustomEmitter);
        CurrentSize += TagText.size();
        TagText = "";
        WereTagsEmittedSinceLastBreak = true;
      } else if (auto LastSpace = TagText.rfind(' ', WrapAt - CurrentSize);
                 LastSpace != llvm::StringRef::npos) {
        // Tag doesn't fit, break on the last space that still does.
        DoxygenToken Tag{ .Type = Token.Type,
                          .Value = TagText.substr(0, LastSpace).str(),
                          .CustomEmitter = Token.CustomEmitter };
        ResultLine->emplace_back(std::move(Tag));
        emitLine(std::move(ResultLine));
        std::tie(ResultLine, CurrentSize) = line(InternalIndentation);
        WereTagsEmittedSinceLastBreak = false;

        auto NextCharacter = std::min(LastSpace + 1, TagText.size());
        TagText = (LastSpace != llvm::StringRef::npos) ?
                    TagText.drop_front(NextCharacter) :
                    "";
      } else if (WereTagsEmittedSinceLastBreak) {
        // Tag doesn't fit and there's no good breaking point, but there
        // are already tags on this line: insert a break and try again.
        emitLine(std::move(ResultLine));
        std::tie(ResultLine, CurrentSize) = line(InternalIndentation);
        WereTagsEmittedSinceLastBreak = false;
      } else {
        // No viable breaking point, make this line longer than expected.
        size_t FirstSpace = TagText.find(' ');
        if (FirstSpace == TagText.npos)
          FirstSpace = TagText.size();
        DoxygenToken Tag{ .Type = Token.Type,
                          .Value = TagText.substr(0, FirstSpace).str(),
                          .CustomEmitter = Token.CustomEmitter };
        ResultLine->emplace_back(std::move(Tag));
        emitLine(std::move(ResultLine));
        std::tie(ResultLine, CurrentSize) = line(InternalIndentation);
        WereTagsEmittedSinceLastBreak = false;

        auto NextCharacter = std::min(FirstSpace + 1, TagText.size());
        TagText = (FirstSpace != llvm::StringRef::npos) ?
                    TagText.drop_front(NextCharacter) :
                    "";
      }
    }
  }

  std::tuple<DoxygenLine, size_t> firstLine() {
    DoxygenLine Result;

    if (!Indicator.empty())
      Result->emplace_back(DoxygenToken::Types::Untagged,
                           Indicator.str() + ' ');

    Result.InternalIndentation = 0;

    return { std::move(Result), Indicator.size() };
  }

  std::tuple<DoxygenLine, size_t> line(size_t IndentationSize = 0) {
    std::tuple<DoxygenLine, size_t> Result = firstLine();

    if (IndentationSize != 0) {
      auto &[Line, ResultSize] = Result;
      Line->emplace_back(DoxygenToken::Types::Indentation,
                         std::string(IndentationSize, ' '));
      Line.InternalIndentation = IndentationSize;
      ResultSize += IndentationSize;
    }

    return Result;
  }

  void emitLine(DoxygenLine &&Line) {
    auto Guard = PTML.initializeOpenTag(ptml::tags::Div);

    revng_assert(!Line.Tags.empty());
    for (DoxygenToken &Tag : Line.Tags)
      Tag.emit(PTML);

    PTML.emitContentNewline();
  }
};

using CCE = ptml::CommentEmitter;

void CCE::emitLineComment(llvm::StringRef Content) {
  auto Tag = PTML.initializeOpenTag(ptml::tags::Span);
  Tag.emitAttribute(ptml::attributes::Token, ptml::tokens::Comment);
  Tag.finalizeOpenTag();

  revng_assert(not Indicators.Line.empty());
  CommentEmitterImpl Impl(PTML,
                          Indicators.Line,
                          WrapAt - PTML.getCurrentIndentation());
  Impl.emit(Content);
}

void CCE::emitDoxygenComment(llvm::StringRef Content) {
  revng_assert(Indicators.Doxygen.has_value(),
               "This comment emitter was initialized without doxygen comment "
               "support.");

  auto Tag = PTML.initializeOpenTag(ptml::tags::Span);
  Tag.emitAttribute(ptml::attributes::Token, ptml::tokens::Comment);
  Tag.finalizeOpenTag();

  revng_assert(not Indicators.Doxygen->empty());
  CommentEmitterImpl Impl(PTML,
                          *Indicators.Doxygen,
                          WrapAt - PTML.getCurrentIndentation());
  Impl.emit(Content);
}

void CCE::emitBlockComment(llvm::StringRef Content) {
  revng_assert(Indicators.Block.has_value(),
               "This comment emitter was initialized without block comment "
               "support.");
  revng_assert(not Indicators.Block->Opening.empty());
  revng_assert(not Indicators.Block->Closing.empty());

  auto Tag = PTML.initializeOpenTag(ptml::tags::Span);
  Tag.emitAttribute(ptml::attributes::Token, ptml::tokens::Comment);
  Tag.finalizeOpenTag();

  // Emitting multi-line block comments *properly* requires information about
  // the exact starting position within the given line, *without* any tags.
  // That is not trivial, so for now just ban them.
  revng_assert(not Content.contains('\n'));

  PTML.emitContent(Indicators.Block->Opening);

  // If closing indicator is found in the comment body, break it by injecting
  // a space before its last character.
  size_t IndicatorPosition = Content.find(Indicators.Block->Closing);
  while (IndicatorPosition != llvm::StringRef::npos) {
    llvm::StringRef ClosingIndicator = Indicators.Block->Closing;
    revng_assert(ClosingIndicator.size() > 1,
                 "Unable to 'break' single-character indicator.");

    PTML.emitContent(Content.substr(0, IndicatorPosition - 1));
    PTML.emitContent(ClosingIndicator.substr(0, ClosingIndicator.size() - 1));
    PTML.emitLiteralContent(" ");
    PTML.emitContent(ClosingIndicator.substr(ClosingIndicator.size() - 1));

    Content = Content.substr(IndicatorPosition + ClosingIndicator.size());
    IndicatorPosition = Content.find(ClosingIndicator);
  }

  PTML.emitContent(Content);

  PTML.emitContent(Indicators.Block->Closing);
}

void CCE::emitDoxygenLineImpl(llvm::StringRef Keyword,
                              llvm::StringRef Content,
                              std::optional<Identifier> Identifier) {
  if (Content.empty())
    return;

  DoxygenLine Line;

  revng_assert(!Keyword.empty());
  Line->emplace_back(DoxygenToken::Types::Keyword, "\\" + std::string(Keyword));
  Line->emplace_back(DoxygenToken::Types::Untagged, " ");
  Line.InternalIndentation = Keyword.size() + 2;

  if (Identifier.has_value()) {
    revng_assert(not Identifier->Name.empty());
    auto &N = Line->emplace_back(DoxygenToken::Types::Identifier,
                                 Identifier->Name.str());
    N.CustomEmitter = Identifier->CustomEmitter;

    Line->emplace_back(DoxygenToken::Types::Untagged, " ");
  }

  Line->emplace_back(DoxygenToken::Types::Untagged, Content.str());
}

} // namespace ptml
