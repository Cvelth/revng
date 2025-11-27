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
    Category, // WIP: better name?

    // ///
    // /// \defgroup Looks like this
    // /// \{
    DoxygenCategoryOpener,

    // /// \}
    DoxygenCategoryCloser,
  };

  void emitComment(llvm::StringRef Content,
                   CommentKind Kind = CommentKind::Line);

  enum class IncludeMode : bool {
    Quote,
    Angle,
  };

  void emitIncludeDirective(llvm::StringRef Content,
                            llvm::StringRef Location,
                            IncludeMode Mode);

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
    None,
    EnumDefinition,
    StructDefinition,
    UnionDefinition,
    FunctionDeclaration,
    FunctionDefinition,
    BlockStatement,
  };

  enum class Delimiter : uint8_t {
    None,
    Braces,
  };

  class Scope {
  public:
    explicit Scope(CTokenEmitter &Emitter,
                   ScopeKind Kind,
                   Delimiter Delimiter,
                   int Indent) :
      Emitter(Emitter), Delimiter(Delimiter), Indent(Indent), Tag() {
      Emitter.enterScopeImpl(Tag, Delimiter, Indent, Kind);
    }

    Scope(const Scope &) = delete;
    Scope &operator=(const Scope &) = delete;

    ~Scope() { Emitter.leaveScopeImpl(Tag, Delimiter, Indent); }

  private:
    CTokenEmitter &Emitter;

    Delimiter Delimiter;
    int Indent;

    ptml::Emitter::TagEmitter Tag;
  };

  [[nodiscard]] Scope
  enterScope(ScopeKind Kind, Delimiter Delimiter, int Indent = 1) {
    return Scope(*this, Kind, Delimiter, Indent);
  }

private:
  void enterScopeImpl(ptml::Emitter::TagEmitter &Tag,
                      Delimiter Delimiter,
                      int Indent,
                      ScopeKind Kind);

  void leaveScopeImpl(ptml::Emitter::TagEmitter &Tag,
                      Delimiter Delimiter,
                      int Indent);
};

} // namespace ptml
