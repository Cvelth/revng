//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "mlir/IR/BuiltinOps.h"

#include "revng/Clift/ModuleVisitor.h"
#include "revng/CliftEmitC/CEmitter.h"
#include "revng/CliftEmitC/Headers.h"
#include "revng/PTML/CTokenEmitter.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"

#include "TypeDefinitionEmitter.h"

ptml::CTokenEmitter::Scope
mlir::clift::emitHeaderPrologue(ptml::CTokenEmitter &PTML) {
  // TODO: accept the corresponding header location as an argument and emit
  //       its definition on the scope tag so that ctrl+click on includes leads
  //       to this file.

  ptml::CTokenEmitter::Scope
    Scope = PTML.enterScope(ptml::CTokenEmitter::ScopeKind::Basic, 0);

  PTML.emitPragmaOnceDirective();
  PTML.emitNewline();

  PTML.emitComment("This header has been generated using rev.ng.",
                   ptml::CTokenEmitter::CommentKind::Category);
  PTML.emitNewline();

  // TODO: emit the license information, revng version information, etc.

  return Scope;
}

void mlir::clift::emitModelIncludes(ptml::CTokenEmitter &PTML) {
  // TODO: attach proper header locations.
  PTML.emitIncludeDirective("stdint.h",
                            "",
                            ptml::CTokenEmitter::IncludeMode::Angle);
  PTML.emitIncludeDirective("stdbool.h",
                            "",
                            ptml::CTokenEmitter::IncludeMode::Angle);
  PTML.emitNewline();
  PTML.emitIncludeDirective("attributes.h",
                            "",
                            ptml::CTokenEmitter::IncludeMode::Quote);
  PTML.emitIncludeDirective("primitive-types.h",
                            "",
                            ptml::CTokenEmitter::IncludeMode::Quote);
  PTML.emitNewline();
}

void mlir::clift::emitModelTypes(ptml::CTokenEmitter &PTML,
                                 const TargetCImplementation &Target,
                                 const mlir::ModuleOp &Module,
                                 TypeSystemEmitterConfiguration Configuration) {
  TypeDefinitionEmitter Emitter(PTML, Target, Configuration);

  auto Guard = PTML.enterDoxygenCategoryScope("Types");

  auto Graph = TypeDependencyGraph::makeModelGraph(Module);

  // In order to improve the printing order, do the visit it in two parts:
  // first only start from nodes without any successors (real roots),
  // only then, resolve potential loops by starting from arbitrary nodes.
  std::set<const TypeDependencyNode *> Emitted;
  for (const auto *Root : Graph.nodes())
    if (not Root->predecessorCount())
      Emitter.emitTypeTree(*Root, Emitted);
  for (const auto *Root : Graph.nodes())
    Emitter.emitTypeTree(*Root, Emitted);
  revng_assert(Graph.size() == Emitted.size());

  PTML.emitNewline();
}

void mlir::clift::emitModelFunctions(ptml::CTokenEmitter &PTML,
                                     const TargetCImplementation &Target,
                                     const mlir::ModuleOp &Module) {
  std::optional<ptml::CTokenEmitter::Scope> Scope = std::nullopt;
  Module->walk([&PTML, &Target, &Scope](mlir::clift::FunctionOp Function) {
    if (pipeline::locationFromString(revng::ranks::Function,
                                     Function.getHandle())) {
      if (not Scope.has_value())
        Scope = PTML.enterDoxygenCategoryScope("Functions");

      CEmitter(PTML, Target).emitFunctionPrototype(Function);
      PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
      PTML.emitNewline();
      PTML.emitNewline();
    }
  });
}

void mlir::clift::emitDynamicModelFunctions(ptml::CTokenEmitter &PTML,
                                            const TargetCImplementation &Target,
                                            const mlir::ModuleOp &Module) {
  std::optional<ptml::CTokenEmitter::Scope> Scope = std::nullopt;

  Module->walk([&PTML, &Target, &Scope](mlir::clift::FunctionOp Function) {
    if (pipeline::locationFromString(revng::ranks::DynamicFunction,
                                     Function.getHandle())) {
      if (not Scope.has_value())
        Scope = PTML.enterDoxygenCategoryScope("Imported Dynamic Functions");

      CEmitter(PTML, Target).emitFunctionPrototype(Function);
      PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
      PTML.emitNewline();
      PTML.emitNewline();
    }
  });
}

void mlir::clift::emitModelSegments(ptml::CTokenEmitter &PTML,
                                    const TargetCImplementation &Target,
                                    const mlir::ModuleOp &Module) {
  std::optional<ptml::CTokenEmitter::Scope> Scope = std::nullopt;

  Module->walk([&PTML, &Target, &Scope](mlir::clift::GlobalVariableOp Segment) {
    auto MaybeLocation = pipeline::locationFromString(revng::ranks::Segment,
                                                      Segment.getHandle());
    revng_assert(MaybeLocation.has_value());

    if (not Scope.has_value())
      Scope = PTML.enterDoxygenCategoryScope("Segments");

    static constexpr auto
      GlobalVariableKind = ptml::CTokenEmitter::EntityKind::GlobalVariable;
    CEmitter(PTML, Target)
      .emitDeclaration(Segment.getType(),
                       CEmitter::DeclaratorInfo{
                         .Identifier = Segment.getName(),
                         .Location = Segment.getHandle(),
                         .Attributes = {},
                         .Kind = GlobalVariableKind,
                         .Parameters = {} });

    PTML.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    PTML.emitNewline();
    PTML.emitNewline();
  });
}

void mlir::clift::emitSingleTypeDefinition(ptml::CTokenEmitter &PTML,
                                           const TargetCImplementation &Target,
                                           const mlir::clift::DefinedType &Type,
                                           TypeSystemEmitterConfiguration
                                             Configuration) {
  TypeDefinitionEmitter Emitter(PTML, Target, Configuration);

  Emitter.emitTypeDefinition(Type);
  PTML.emitNewline();
}
