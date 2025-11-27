#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/StringRef.h"

#include "revng/PTML/Emitter.h"
#include "revng/Support/CTarget.h"

namespace ptml {

class CTokenEmitter {
  ptml::Emitter PTML;

public:
  explicit CTokenEmitter(llvm::raw_ostream &OS, ptml::Tagging Tags) :
    PTML(OS, Tags) {}

  void emitSpace() { PTML.emitLiteralContent(" "); }

  void emitNewline() { PTML.emitContentNewline(); }

  enum class Keyword {
    Auto,
    Bool,
    Break,
    Case,
    Char,
    Const,
    Continue,
    Default,
    Do,
    Double,
    Else,
    Enum,
    Extern,
    False,
    For,
    Goto,
    If,
    Inline,
    Int,
    Long,
    Register,
    Return,
    Short,
    Signed,
    Sizeof,
    Static,
    Struct,
    Switch,
    True,
    Typedef,
    Union,
    Unsigned,
    Void,
    Volatile,
    While,
  };

  void emitKeyword(Keyword K);

  enum class Punctuator {
    Colon,
    Comma,
    Dot,
    Equals,
    LeftBrace,
    LeftBracket,
    LeftParenthesis,
    RightBrace,
    RightBracket,
    RightParenthesis,
    Semicolon,
    Star,
  };

  void emitPunctuator(Punctuator P);

  enum class Operator {
    Ampersand,
    AmpersandAmpersand,
    AmpersandEquals,
    Arrow,
    Caret,
    CaretEquals,
    Colon,
    Comma,
    Dot,
    Equals,
    EqualsEquals,
    Exclaim,
    ExclaimEquals,
    Greater,
    GreaterEquals,
    GreaterGreater,
    GreaterGreaterEquals,
    LeftBracket,
    LeftParenthesis,
    Less,
    LessEquals,
    LessLess,
    LessLessEquals,
    Minus,
    MinusEquals,
    MinusMinus,
    Percent,
    PercentEquals,
    Pipe,
    PipeEquals,
    PipePipe,
    Plus,
    PlusEquals,
    PlusPlus,
    Question,
    RightBracket,
    RightParenthesis,
    Slash,
    SlashEquals,
    Star,
    StarEquals,
    Tilde,
  };

  void emitOperator(Operator O);

  enum class EntityKind {
    Primitive,
    Typedef,

    Enum,
    Enumerator,

    Struct,
    Union,
    Field,

    GlobalVariable,
    LocalVariable,

    Function,
    FunctionParameter,

    Label,

    Attribute,
    AttributeArgument,

    Macro,
  };

  enum class IdentifierKind : bool {
    Reference,
    Definition,
  };

  /// \pre \param Identifier matches `[_a-zA-Z][_a-zA-Z0-9]*`.
  void emitIdentifier(llvm::StringRef Identifier,
                      llvm::StringRef Location,
                      EntityKind Kind,
                      IdentifierKind IsDefinition);

  void emitPrimitive(llvm::StringRef Name,
                     IdentifierKind IsDefinition = IdentifierKind::Reference);

  void emitMacro(llvm::StringRef Name,
                 IdentifierKind IsDefinition = IdentifierKind::Reference);
  void
  emitMacroArgument(llvm::StringRef MacroName,
                    llvm::StringRef ArgumentName,
                    IdentifierKind IsDefinition = IdentifierKind::Reference);

  /// \pre \param Identifier matches `[_a-zA-Z][_a-zA-Z0-9]*`.
  void emitLiteralIdentifier(llvm::StringRef Identifier);

  // TODO: There is currently no API for emitting character literals, because
  //       there are no Clift users of such an API. Whenever support for
  //       emitting character literals is needed, another function should be
  //       added for that purpose.

  /// \pre \param Radix must be one of 2, 8, 10 or 16.
  void
  emitIntegerLiteral(llvm::APSInt Value, CIntegerKind Type, unsigned Radix);

  void emitUntypedIntegerLiteral(uint64_t Value);
  void emitUntypedHexLiteral(uint64_t Value);

private:
  void emitStringLiteralImpl(llvm::StringRef Content,
                             bool ShouldEmitQuotationMarks);

public:
  void emitStringLiteral(llvm::StringRef Content) {
    emitStringLiteralImpl(Content, true);
  }
  void emitUnquotedStringLiteral(llvm::StringRef Content) {
    emitStringLiteralImpl(Content, false);
  }

  enum class CommentKind : bool {
    // // Looks like this
    Line,

    // /* Looks like this */
    Block
  };

  // The following function should *never* be called directly.
  //
  // When you need to emit a comment, use `CCommentEmitter` built on top of it.
  void emitComment(llvm::StringRef Content, CommentKind Kind);

  enum class IncludeMode : bool {
    Quote,
    Angle,
  };

  void emitIncludeDirective(llvm::StringRef Content,
                            llvm::StringRef Location,
                            IncludeMode Mode);

  enum class ScopeKind : uint8_t {
    /// Doesn't emit anything. Is provided as a handy way of controlling
    /// indentation without showing up in the output.
    IndentOnly,

    /// Doesn't emit anything beyond the basic `<div>...</div>` pair.
    Basic,

    /// These are the same as \ref Basic except they also set
    /// `ptml::attributes::Scope` to an appropriate value
    FunctionDeclaration,

    /// The same as \ref Basic but it also emits the PTML attribute allowing
    /// this region to be folded.
    Foldable,

    /// The same as \ref Foldable except a brace pair ({}) is also emitted.
    BlockStatement,

    /// These are the same as \ref BlockStatement except they also set
    /// `ptml::attributes::Scope` to an appropriate value
    EnumDefinition,
    FunctionDefinition,
    StructDefinition,
    UnionDefinition,
  };

  void emitScopeOpener(ScopeKind Kind) {
    switch (Kind) {
    case ScopeKind::IndentOnly:
    case ScopeKind::Basic:
    case ScopeKind::Foldable:
    case ScopeKind::FunctionDeclaration:
      return;

    case ScopeKind::BlockStatement:
    case ScopeKind::EnumDefinition:
    case ScopeKind::FunctionDefinition:
    case ScopeKind::StructDefinition:
    case ScopeKind::UnionDefinition:
      emitPunctuator(Punctuator::LeftBrace);
      return;

    default:
      revng_abort("Unknown scope kind");
    }
  }

  void emitScopeCloser(ScopeKind Kind) {
    switch (Kind) {
    case ScopeKind::IndentOnly:
    case ScopeKind::Basic:
    case ScopeKind::Foldable:
    case ScopeKind::FunctionDeclaration:
      return;

    case ScopeKind::BlockStatement:
    case ScopeKind::EnumDefinition:
    case ScopeKind::FunctionDefinition:
    case ScopeKind::StructDefinition:
    case ScopeKind::UnionDefinition:
      emitPunctuator(Punctuator::RightBrace);
      return;

    default:
      revng_abort("Unknown scope kind");
    }
  }

  void indent(int64_t LevelDifference) { PTML.indent(LevelDifference); }

  class Scope {
  public:
    explicit Scope(CTokenEmitter &Emitter, ScopeKind Kind, int Indent) :
      Emitter(Emitter), Kind(Kind), Indent(Indent) {
      Emitter.enterScopeImpl(Tag, Indent, Kind);
    }

    Scope(const Scope &) = delete;
    Scope &operator=(const Scope &) = delete;

    ~Scope() {
      if (Tag.isOpen())
        Emitter.leaveScopeImpl(Tag, Indent, Kind);
    }

  private:
    CTokenEmitter &Emitter;

    ScopeKind Kind;
    int Indent;

    ptml::Emitter::TagEmitter Tag;
  };

  [[nodiscard]] Scope enterScope(ScopeKind Kind, int Indent = 1) {
    return Scope(*this, Kind, Indent);
  }

  enum class RegionKind : uint8_t {
    Expression,
  };

  class Region {
    ptml::Emitter::TagEmitter Tag;

  public:
    explicit Region(CTokenEmitter &Emitter,
                    RegionKind Kind,
                    llvm::StringRef Location) {
      Emitter.enterRegionImpl(Tag, Kind, Location);
    }

    Region(const Region &) = delete;
    Region &operator=(const Region &) = delete;
  };

  [[nodiscard]] Region enterRegion(RegionKind Kind, llvm::StringRef Location) {
    return Region(*this, Kind, Location);
  }

private:
  void
  enterScopeImpl(ptml::Emitter::TagEmitter &Tag, int Indent, ScopeKind Kind);

  void
  leaveScopeImpl(ptml::Emitter::TagEmitter &Tag, int Indent, ScopeKind Kind);

  void enterRegionImpl(ptml::Emitter::TagEmitter &Tag,
                       RegionKind Kind,
                       llvm::StringRef Location);
};

} // namespace ptml
