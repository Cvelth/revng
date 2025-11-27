#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/PostOrderIterator.h"

#include "revng/CliftEmitC/CEmitter.h"
#include "revng/CliftEmitC/Headers.h"
#include "revng/Model/NameBuilder.h"
#include "revng/PTML/Constants.h"

#include "TypeDependencyGraph.h"

inline Logger TypePrinterLog{ "clift-type-definition-printer" };

class TypeDefinitionEmitter : public mlir::clift::CEmitter {
private:
  mlir::clift::TypeSystemEmitterConfiguration Configuration;

public:
  TypeDefinitionEmitter(mlir::clift::CEmitter &Emitter,
                        mlir::clift::TypeSystemEmitterConfiguration
                          Configuration) :
    mlir::clift::CEmitter(Emitter), Configuration(Configuration) {}

  TypeDefinitionEmitter(ptml::CTokenEmitter &PTML,
                        const TargetCImplementation &Target,
                        mlir::clift::TypeSystemEmitterConfiguration
                          Configuration) :
    mlir::clift::CEmitter(PTML, Target), Configuration(Configuration) {}

private:
  void emitTypeKeyword(mlir::clift::DefinedType Type) {
    if (mlir::isa<mlir::clift::EnumType>(Type))
      PTML.emitKeyword(ptml::CTokenEmitter::Keyword::Enum);

    else if (mlir::isa<mlir::clift::StructType>(Type))
      PTML.emitKeyword(ptml::CTokenEmitter::Keyword::Struct);

    else if (mlir::isa<mlir::clift::UnionType>(Type))
      PTML.emitKeyword(ptml::CTokenEmitter::Keyword::Union);

    else if (mlir::isa<mlir::clift::TypedefType>(Type)
             || mlir::isa<mlir::clift::FunctionType>(Type))
      PTML.emitKeyword(ptml::CTokenEmitter::Keyword::Typedef);

    else
      revng_abort("Unsupported defined type.");
  }

  ptml::TagEmitter markAsCommentable(llvm::StringRef Location) {
    auto Tag = PTML.initializeOpenTag(ptml::tags::Span);
    Tag.emitAttribute(ptml::attributes::ActionContextLocation, Location);
    Tag.emitListAttribute(ptml::attributes::AllowedActions,
                          { ptml::actions::Comment });
    Tag.finalizeOpenTag();
    return Tag;
  }

public:
  void emitForwardDeclaration(mlir::clift::DefinedType Type) {
    revng_assert(not CEmitter::isDeclarationTheSameAsDefinition(Type));

    PTML.emitKeyword(ptml::CTokenEmitter::Keyword::Typedef);
    PTML.emitSpace();
    emitTypeKeyword(Type);
    PTML.emitSpace();
    PTML.emitAttribute<"_PACKED">();
    PTML.emitSpace();
    PTML.emitIdentifier(Type.getName(),
                        Type.getHandle(),
                        chooseEntityKind(Type),
                        ptml::CTokenEmitter::IdentifierKind::Reference);
    PTML.emitSpace();
    PTML.emitIdentifier(Type.getName(),
                        Type.getHandle(),
                        chooseEntityKind(Type),
                        ptml::CTokenEmitter::IdentifierKind::Reference);
    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitTypedefDefinition(mlir::clift::TypedefType Typedef) {
    auto Guard = markAsCommentable(Typedef.getHandle());

    // TODO: emit model comment.

    emitTypeKeyword(Typedef);
    PTML.emitSpace();

    emitDeclaration(Typedef.getUnderlyingType(),
                    mlir::clift::CEmitter::DeclaratorInfo{
                      .Identifier = Typedef.getName(),
                      .Location = Typedef.getHandle(),

                      // FUTURE-WIP: should i be passing something in here?
                      .Attributes = {},

                      .Kind = ptml::CTokenEmitter::EntityKind::Typedef,
                    });
    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitFunctionTypedef(mlir::clift::FunctionType Function) {
    if (Function.getName().empty()) {
      revng_abort("DEBUG");
      revng_assert(pipeline::locationFromString(revng::ranks::HelperFunction,
                                                Function.getHandle()));
      // Skip helper typedefs.
      return;
    }

    auto Guard = markAsCommentable(Function.getHandle());

    // TODO: emit model comment.

    emitTypeKeyword(Function);
    PTML.emitSpace();

    emitDeclaration(Function,
                    mlir::clift::CEmitter::DeclaratorInfo{
                      .Identifier = Function.getName(),
                      .Location = Function.getHandle(),
                      .Attributes = {},
                      .Kind = chooseEntityKind(Function),
                      .Parameters = {},
                    });
    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitTypeDeclaration(mlir::clift::DefinedType Type) {
    if (not CEmitter::isDeclarationTheSameAsDefinition(Type)) {
      emitForwardDeclaration(Type);

    } else if (auto Typedef = mlir::dyn_cast<mlir::clift::TypedefType>(Type)) {
      emitTypedefDefinition(Typedef);

    } else if (auto
                 Function = mlir::dyn_cast<mlir::clift::FunctionType>(Type)) {
      emitFunctionTypedef(Function);

    } else {
      Type.dump();
      revng_abort("Unknown defined type.");
    }
  }

private:
  static std::string paddingFieldName(uint64_t CurrentOffset) {
    // TODO: this discards the prefix configuration option.
    //       We should fix this after the configuration is separate from the
    //       model

    model::CNameBuilder Builder(model::Binary{});
    return Builder.paddingFieldName(CurrentOffset);
  }

public:
  void emitPaddingField(uint64_t CurrentOffset, uint64_t NextOffset) {

    revng_assert(CurrentOffset <= NextOffset);
    if (CurrentOffset == NextOffset)
      return; // There is no padding

    static constexpr auto Unsigned = model::PrimitiveKind::Unsigned;
    PTML.emitPrimitive(model::PrimitiveType::getCName(Unsigned, 1));
    PTML.emitSpace();

    PTML.emitIdentifier(paddingFieldName(CurrentOffset),
                        "",
                        ptml::CTokenEmitter::EntityKind::Field,
                        ptml::CTokenEmitter::IdentifierKind::Definition);
    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::LeftBracket);
    PTML.emitSimpleIntegerLiteral(NextOffset - CurrentOffset);
    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::RightBracket);
    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitStructDefinition(mlir::clift::StructType Struct) {
    {
      auto Guard = markAsCommentable(Struct.getHandle());

      // TODO: emit model comment.

      emitTypeKeyword(Struct);
      PTML.emitSpace();
      PTML.emitAttribute<"_PACKED">();
      PTML.emitSpace();

      // TODO: conditionally emit `_CAN_CONTAIN_CODE`.

      PTML.emitAnnotation<"_SIZE">(Struct.getSize());
      PTML.emitSpace();

      PTML.emitIdentifier(Struct.getName(),
                          Struct.getHandle(),
                          chooseEntityKind(Struct),
                          ptml::CTokenEmitter::IdentifierKind::Definition);
      PTML.emitSpace();
    }

    {
      auto Sc = PTML
                  .enterScope(ptml::CTokenEmitter::ScopeKind::StructDefinition);
      PTML.emitNewline();

      uint64_t PreviousOffset = 0;
      for (const auto &Field : Struct.getFields()) {
        if (not Configuration.SupressExplicitPadding)
          emitPaddingField(PreviousOffset, Field.getOffset());

        auto Guard = markAsCommentable(Field.getHandle());

        // TODO: emit model comment.

        emitDeclaration(Field.getType(),
                        mlir::clift::CEmitter::DeclaratorInfo{
                          .Identifier = Field.getName(),
                          .Location = Field.getHandle(),

                          // FUTURE-WIP: should I be passing something in here?
                          .Attributes = {},

                          .Kind = ptml::CTokenEmitter::EntityKind::Field,
                        });

        // TODO: is an automatic name emitted? Or a custom one?
        if (true) {
          PTML.emitSpace();
          PTML.emitAnnotation<"_STARTS_AT">(Field.getOffset());
        }

        PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
        PTML.emitNewline();

        PreviousOffset = Field.getOffset() + Field.getType().getByteSize();
      }
    }

    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitUnionDefinition(mlir::clift::UnionType Union) {
    {
      auto Guard = markAsCommentable(Union.getHandle());

      // TODO: emit model comment.

      emitTypeKeyword(Union);
      PTML.emitSpace();
      PTML.emitAttribute<"_PACKED">();
      PTML.emitSpace();

      PTML.emitIdentifier(Union.getName(),
                          Union.getHandle(),
                          chooseEntityKind(Union),
                          ptml::CTokenEmitter::IdentifierKind::Definition);
      PTML.emitSpace();
    }

    {
      auto Sc = PTML
                  .enterScope(ptml::CTokenEmitter::ScopeKind::UnionDefinition);
      PTML.emitNewline();

      for (const auto &Field : Union.getFields()) {
        auto Guard = markAsCommentable(Field.getHandle());

        // TODO: emit model comment.

        emitDeclaration(Field.getType(),
                        mlir::clift::CEmitter::DeclaratorInfo{
                          .Identifier = Field.getName(),
                          .Location = Field.getHandle(),

                          // FUTURE-WIP: should I be passing something in here?
                          .Attributes = {},

                          .Kind = ptml::CTokenEmitter::EntityKind::Field,
                        });

        PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
        PTML.emitNewline();
      }
    }

    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitEnumDefinition(mlir::clift::EnumType Enum) {
    {
      auto Guard = markAsCommentable(Enum.getHandle());

      // TODO: emit model comment.

      emitTypeKeyword(Enum);
      PTML.emitSpace();
      {
        auto AnnotationGuard = PTML.emitComplexAnnotation<"_ENUM_UNDERLYING">();
        emitType(Enum.getUnderlyingType());
      }
      PTML.emitSpace();
      PTML.emitAttribute<"_PACKED">();
      PTML.emitSpace();

      PTML.emitIdentifier(Enum.getName(),
                          Enum.getHandle(),
                          chooseEntityKind(Enum),
                          ptml::CTokenEmitter::IdentifierKind::Definition);
      PTML.emitSpace();
    }

    {
      auto
        Scope = PTML.enterScope(ptml::CTokenEmitter::ScopeKind::EnumDefinition);
      PTML.emitNewline();

      for (const auto &Entry : Enum.getFields()) {
        auto Guard = markAsCommentable(Entry.getHandle());

        // TODO: emit model comment.

        PTML.emitIdentifier(Entry.getName(),
                            Entry.getHandle(),
                            ptml::CTokenEmitter::EntityKind::Enumerator,
                            ptml::CTokenEmitter::IdentifierKind::Definition);
        PTML.emitSpace();
        PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Equals);
        PTML.emitSpace();

        PTML.emitSimpleHexLiteral(Entry.getRawValue());

        PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
        PTML.emitNewline();
      }

      if (Configuration.EnablePrintingOfTheMaximumEnumValue) {
        // We have to make the enum of the correct size of the underlying type
        auto ByteSize = Enum.getByteSize();
        revng_assert(ByteSize <= 8);
        size_t FullMask = std::numeric_limits<size_t>::max();
        size_t MaxBitPatternInEnum = (ByteSize == 8) ?
                                       FullMask :
                                       ((FullMask)
                                        xor (FullMask << (8 * ByteSize)));

        // TODO: pull the prefix from the configuration when it's available
        //       without pulling in the model dependency.
        static constexpr llvm::StringRef Prefix = "enum_max_value_";

        namespace ranks = revng::ranks;
        auto EnumLocation = *pipeline::locationFromString(ranks::TypeDefinition,
                                                          Enum.getHandle());
        auto EntryLocation = EnumLocation.extend(ranks::EnumEntry,
                                                 MaxBitPatternInEnum);

        PTML.emitIdentifier(Prefix.str() + Enum.getName().str(),
                            EntryLocation.toString(),
                            ptml::CTokenEmitter::EntityKind::Enumerator,
                            ptml::CTokenEmitter::IdentifierKind::Definition);
        PTML.emitSpace();
        PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Equals);
        PTML.emitSpace();

        PTML.emitSimpleHexLiteral(MaxBitPatternInEnum);

        PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
        PTML.emitNewline();
      }
    }

    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
  }

  void emitTypeDefinition(mlir::clift::DefinedType Type) {
    if (isDeclarationTheSameAsDefinition(Type)) {
      emitTypeDeclaration(Type);
      PTML.emitNewline();
      return;

    } else if (auto Struct = mlir::dyn_cast<mlir::clift::StructType>(Type)) {
      emitStructDefinition(Struct);

    } else if (auto Union = mlir::dyn_cast<mlir::clift::UnionType>(Type)) {
      emitUnionDefinition(Union);

    } else if (auto Enum = mlir::dyn_cast<mlir::clift::EnumType>(Type)) {
      emitEnumDefinition(Enum);

    } else {
      Type.dump();
      revng_abort("Unknown defined type.");
    }
  }

public:
  void
  emitTypeTree(const mlir::clift::TypeDependencyNode &Root,
               std::set<const mlir::clift::TypeDependencyNode *> &Emitted) {
    revng_log(TypePrinterLog,
              "Starting a post order visit from:" << getNodeLabel(&Root));

    bool SkipTheRest = false;

    size_t NodesEmittedAlready = Emitted.size();
    for (const auto *Node : llvm::post_order_ext(&Root, Emitted)) {
      LoggerIndent PostOrderIndent{ TypePrinterLog };

      if (SkipTheRest) {
        revng_log(TypePrinterLog,
                  "skipping (TypeToOmit): " << getNodeLabel(Node));
        continue;
      } else {
        revng_log(TypePrinterLog, "visiting: " << getNodeLabel(Node));
      }

      mlir::clift::DefinedType Definition = Node->T;

      if (Node->isDeclaration()) {
        revng_log(TypePrinterLog, "Declaration");
        emitTypeDeclaration(Definition);

      } else {
        revng_log(TypePrinterLog, "Definition");
        revng_assert(Node->isDefinition());
        revng_assert(not isDeclarationTheSameAsDefinition(Definition));

        revng_assert(not Definition.getHandle().empty());
        if (Definition.getHandle() != Configuration.TypeToOmit)
          emitTypeDefinition(Definition);
        else
          SkipTheRest = true;
      }
    }

    if (NodesEmittedAlready != Emitted.size())
      PTML.emitNewline();

    revng_log(TypePrinterLog, "Root is done: " << getNodeLabel(&Root));
  }
};
