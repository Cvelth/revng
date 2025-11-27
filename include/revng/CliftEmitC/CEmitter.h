#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Clift/Clift.h"
#include "revng/PTML/CTokenEmitter.h"
#include "revng/Support/Annotations.h"
#include "revng/Support/CTarget.h"

namespace mlir::clift {

/// Base class with common utilities for emitters emitting C from Clift.
class CEmitter {
protected:
  using CTE = ptml::CTokenEmitter;

  ptml::CTokenEmitter &Tokens;
  const TargetCImplementation &Target;

public:
  explicit CEmitter(ptml::CTokenEmitter &Emitter,
                    const TargetCImplementation &Target) :
    Tokens(Emitter), Target(Target) {}

  //===------------------------------- Types ------------------------------===//

  void emitPrimitiveType(PrimitiveKind Kind, uint64_t Size);

  void emitPrimitiveType(PrimitiveType Type) {
    emitPrimitiveType(Type.getKind(), Type.getSize());
  }

  void emitType(ValueType Type);

  //===---------------------------- Attributes ----------------------------===//

  static bool isValidAttributeArray(mlir::ArrayAttr Array);
  mlir::ArrayAttr getDeclarationOpAttributes(mlir::Operation *Op);

  void emitAttribute(AttributeAttr Attribute);
  void emitAttributes(mlir::ArrayAttr Attributes);

  //===---------------------------- Prototype -----------------------------===//

  void emitFunctionPrototype(FunctionOp Function);

  //===--------------------------- Declarations ---------------------------===//

  /// Describes a function parameter declarator.
  struct ParameterDeclaratorInfo {
    llvm::StringRef Identifier;
    llvm::StringRef Location;
    mlir::ArrayAttr Attributes;
  };

  /// Describes a declarator. This can be any function or variable declarator,
  /// including a function parameter declarator. When emitting a function
  /// declaration, the parameters declarators array must contain entries for
  /// each parameter of the outermost function type.
  struct DeclaratorInfo {
    llvm::StringRef Identifier;
    llvm::StringRef Location;
    mlir::ArrayAttr Attributes;
    CTE::EntityKind Kind;

    std::optional<llvm::ArrayRef<ParameterDeclaratorInfo>> Parameters;
  };

  /// Emit a function or variable declaration of the specified type.
  void emitDeclaration(ValueType Type, DeclaratorInfo const &Declarator);

private:
  class DeclarationEmitter;

public:
  //===---------------------------- Attributes ----------------------------===//
  template<ConstexprString Macro>
  void emitAttribute() {
    constexpr std::optional Attribute = ptml::Attributes.getAttribute<Macro>();
    if constexpr (Attribute) {
      Tokens.emitMacro(Attribute->Macro);
    } else {
      static_assert(value_always_false_v<Macro>, "Unknown attribute.");
    }
  }

  template<ConstexprString Macro>
  void emitAnnotation(std::string_view Value) {
    constexpr std::optional // formatting
      Annotation = ptml::Attributes.getAnnotation<Macro>();
    if constexpr (Annotation) {
      Tokens.emitMacro(Annotation->Macro);
      Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::LeftParenthesis);
      Tokens.emitUnquotedStringLiteral(Value);
      Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::RightParenthesis);
    } else {
      static_assert(value_always_false_v<Macro>, "Unknown annotation.");
    }
  }

  template<ConstexprString Macro>
  void emitAnnotation(uint64_t Value) {
    emitAnnotation<Macro>(std::to_string(Value));
  }

  struct ComplexAnnotationGuard {
    ptml::CTokenEmitter &PTML;

    ~ComplexAnnotationGuard() {
      PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::RightParenthesis);
    }
  };
  template<ConstexprString Macro>
  ComplexAnnotationGuard emitComplexAnnotation() {
    constexpr std::optional // formatting
      Annotation = ptml::Attributes.getAnnotation<Macro>();
    if constexpr (Annotation) {
      Tokens.emitMacro(Annotation->Macro);
      Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::LeftParenthesis);
      return ComplexAnnotationGuard{ Tokens };
    } else {
      static_assert(value_always_false_v<Macro>, "Unknown annotation.");
    }
  }

public:
  //===--------------------------- Other Helpers --------------------------===//

  static ptml::CTokenEmitter::EntityKind
  chooseEntityKind(mlir::clift::DefinedType Type);
};

} // namespace mlir::clift
