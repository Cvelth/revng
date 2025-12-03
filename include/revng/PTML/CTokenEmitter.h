#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/StringRef.h"

#include "revng/PTML/Emitter.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Support/Annotations.h"
#include "revng/Support/CTarget.h"

namespace ptml {

class CTokenEmitter {
  ptml::Emitter PTML;

public:
  explicit CTokenEmitter(llvm::raw_ostream &OS, ptml::Tagging Tags) :
    PTML(OS, Tags) {}

  [[nodiscard]] TagEmitter initializeOpenTag(llvm::StringRef Tag) {
    return PTML.initializeOpenTag(Tag);
  }

  void emitSpace() { PTML.emitLiteralContent(" "); }

  void emitNewline() { PTML.emitContentNewline(); }

  // TODO: group this with something else! Punctuators maybe?
  void emitBackslash() {
    // TODO: does this need escaping in ptml?
    PTML.emitLiteralContent("\\");
  }

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
                     IdentifierKind IsDefinition = IdentifierKind::Reference) {
    emitIdentifier(Name,
                   pipeline::locationString(revng::ranks::PrimitiveType,
                                            Name.str()),
                   EntityKind::Primitive,
                   IsDefinition);
  }

  void emitMacro(llvm::StringRef Name,
                 IdentifierKind IsDefinition = IdentifierKind::Reference) {
    emitIdentifier(Name,
                   pipeline::locationString(revng::ranks::Macro, Name.str()),
                   EntityKind::Macro,
                   IsDefinition);
  }

  void
  emitMacroArgument(llvm::StringRef MacroName,
                    llvm::StringRef ArgumentName,
                    IdentifierKind IsDefinition = IdentifierKind::Reference) {
    emitIdentifier(ArgumentName,
                   pipeline::locationString(revng::ranks::MacroArgument,
                                            MacroName.str(),
                                            ArgumentName.str()),
                   EntityKind::FunctionParameter,
                   IsDefinition);
  }

  /// \pre \param Identifier matches `[_a-zA-Z][_a-zA-Z0-9]*`.
  void emitLiteralIdentifier(llvm::StringRef Identifier);

  // TODO: There is currently no API for emitting character literals, because
  //       there are no Clift users of such an API. Whenever support for
  //       emitting character literals is needed, another function should be
  //       added for that purpose.

  /// \pre \param Radix must be one of 2, 8, 10 or 16.
  void
  emitIntegerLiteral(llvm::APSInt Value, CIntegerKind Type, unsigned Radix);

  void emitSimpleIntegerLiteral(int64_t Value);
  void emitSimpleHexLiteral(int64_t Value);

  void emitStringLiteralImpl(llvm::StringRef Content,
                             llvm::StringRef Delimiter);
  void emitStringLiteral(llvm::StringRef Content) {
    emitStringLiteralImpl(Content, "\"");
  }

  enum class CommentKind : uint8_t {
    // // Looks like this
    Line,

    // /* Looks like this */
    Block,

    // //
    // // Looks like this
    // //
    Category,
    // FUTURE-WIP: better name?

    // ///
    // /// \defgroup Looks like this
    // /// \{
    DoxygenCategoryOpener,

    // /// \}
    DoxygenCategoryCloser,
  };

  void emitComment(llvm::StringRef Content,
                   CommentKind Kind = CommentKind::Line);

  enum class PreprocessorDirective : uint8_t {
    Include,
    Pragma,
  };

  void emitDirective(PreprocessorDirective Directive);

  enum class IncludeMode : bool {
    Quote,
    Angle,
  };

  void emitIncludeDirective(llvm::StringRef Content,
                            llvm::StringRef Location,
                            IncludeMode Mode);

  void emitPragmaDirective(llvm::StringRef Content);
  void emitPragmaOnceDirective() { return emitPragmaDirective("once"); }

public:
  template<ConstexprString Macro>
  void emitAttribute() {
    constexpr std::optional Attribute = Attributes.getAttribute<Macro>();
    if constexpr (Attribute) {
      emitMacro(Attribute->Macro);
    } else {
      static_assert(value_always_false_v<Macro>, "Unknown attribute.");
    }
  }

  template<ConstexprString Macro>
  void emitAnnotation(std::string_view Value) {
    constexpr std::optional Annotation = Attributes.getAnnotation<Macro>();
    if constexpr (Annotation) {
      emitMacro(Annotation->Macro);
      emitPunctuator(ptml::CTokenEmitter::Punctuator::LeftParenthesis);
      emitStringLiteralImpl(Value, ""); // TODO: we can do better here
      emitPunctuator(ptml::CTokenEmitter::Punctuator::RightParenthesis);
    } else {
      static_assert(value_always_false_v<Macro>, "Unknown annotation.");
    }
  }

  template<ConstexprString Macro>
  void emitAnnotation(uint64_t Value) {
    emitAnnotation<Macro>(std::to_string(Value));
  }

  struct ComplexAnnotationGuard {
    CTokenEmitter &PTML;

    ~ComplexAnnotationGuard() {
      PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::RightParenthesis);
    }
  };
  template<ConstexprString Macro>
  ComplexAnnotationGuard emitComplexAnnotation() {
    constexpr std::optional Annotation = Attributes.getAnnotation<Macro>();
    if constexpr (Annotation) {
      emitMacro(Annotation->Macro);
      emitPunctuator(ptml::CTokenEmitter::Punctuator::LeftParenthesis);
      return ComplexAnnotationGuard{ *this };
    } else {
      static_assert(value_always_false_v<Macro>, "Unknown annotation.");
    }
  }

public:
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

    /// The following are the same as \ref Foldable, except they emit additional
    /// custom tokens before and after the scope.
    DoxygenCategoryComment,
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

    case ScopeKind::DoxygenCategoryComment:
      // TODO: consider wrapping the opening comment under here instead of
      //       the dedicated helper.
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

    case ScopeKind::DoxygenCategoryComment:
      emitComment("", CommentKind::DoxygenCategoryCloser);
      return;

    default:
      revng_abort("Unknown scope kind");
    }
  }

  void indent(int64_t LevelDifference) { PTML.indent(LevelDifference); }

  class Scope {
  public:
    explicit Scope(CTokenEmitter &Emitter, ScopeKind Kind, int Indent) :
      Emitter(Emitter), Kind(Kind), Indent(Indent), Tag() {

      Emitter.enterScopeImpl(Tag, Indent, Kind);
    }

    Scope(const Scope &) = delete;
    Scope &operator=(const Scope &) = delete;

    Scope(Scope &&Another) :
      Emitter(Another.Emitter),
      Kind(Another.Kind),
      Indent(Another.Indent),
      Tag(std::move(Another.Tag)) {

      Another.Tag = {};
    }

    Scope &operator=(Scope &&Another) {
      revng_assert(&Emitter == &Another.Emitter);

      Kind = Another.Kind;
      Indent = Another.Indent;
      Tag = std::move(Another.Tag);

      Another.Tag = {};

      return *this;
    }

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

private:
  void
  enterScopeImpl(ptml::Emitter::TagEmitter &Tag, int Indent, ScopeKind Kind);

  void
  leaveScopeImpl(ptml::Emitter::TagEmitter &Tag, int Indent, ScopeKind Kind);

public:
  Scope enterDoxygenCategoryScope(llvm::StringRef Content) {
    emitComment(Content,
                ptml::CTokenEmitter::CommentKind::DoxygenCategoryOpener);

    return enterScope(ScopeKind::DoxygenCategoryComment, 0);
  }
};

} // namespace ptml
