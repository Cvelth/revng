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
#include "TypeDependencyGraph.h"

void mlir::clift::emitHeaderPrologue(ptml::CTokenEmitter &Tokens) {
  Tokens.emitPragmaOnceDirective();
  Tokens.emitNewline();

  // TODO: load from configuration when it's available.
  constexpr static uint64_t WrapCommentsAt = 80;

  auto Comments = Tokens.comments(WrapCommentsAt);
  Comments.emitCategoryComment("This header has been generated using rev.ng.");
  Tokens.emitNewline();

  // TODO: emit the license information, revng version information, etc.
}

void mlir::clift::emitModelIncludes(ptml::CTokenEmitter &Tokens) {
  // TODO: attach proper header locations.
  Tokens.emitIncludeDirective("attributes.h",
                              "",
                              ptml::CTokenEmitter::IncludeMode::Quote);
  Tokens.emitIncludeDirective("primitive-types.h",
                              "",
                              ptml::CTokenEmitter::IncludeMode::Quote);
  Tokens.emitNewline();
}

static void emitTypeGraph(const mlir::clift::TypeDependencyGraph &Graph,
                          ptml::CTokenEmitter &Tokens,
                          const TargetCImplementation &Target,
                          mlir::clift::TypeEmitterConfiguration Configuration) {
  TypeDefinitionEmitter Emitter(Tokens, Target, Configuration);

  // In order to improve the printing order, do the visit it in two parts:
  // first only start from nodes without any successors (real roots),
  // only then, resolve potential loops by starting from arbitrary nodes.
  std::unordered_set<const mlir::clift::TypeDependencyNode *> Emitted;
  for (const auto *Root : Graph.nodes())
    if (not Root->predecessorCount())
      Emitter.emitTypeTree(*Root, Emitted);
  for (const auto *Root : Graph.nodes())
    Emitter.emitTypeTree(*Root, Emitted);
  revng_assert(Graph.size() == Emitted.size());

  Tokens.emitNewline();
}

void mlir::clift::emitModelTypes(ptml::CTokenEmitter &Tokens,
                                 const TargetCImplementation &Target,
                                 mlir::ModuleOp Module,
                                 TypeEmitterConfiguration Configuration) {
  auto Graph = TypeDependencyGraph::makeModelGraph(Module);

  if (not Graph.empty()) {
    // TODO: load from configuration when it's available.
    constexpr static uint64_t WrapCommentsAt = 80;

    auto Comments = Tokens.comments(WrapCommentsAt);
    Comments.emitCategoryComment("Types");
  }

  emitTypeGraph(Graph, Tokens, Target, Configuration);
}

void mlir::clift::emitModelFunctions(ptml::CTokenEmitter &Tokens,
                                     const TargetCImplementation &Target,
                                     mlir::ModuleOp Module) {
  CEmitter Emitter(Tokens, Target);

  bool CommentEmitted = false;
  Module->walk([&Tokens,
                &Emitter,
                &CommentEmitted](mlir::clift::FunctionOp Function) {
    if (pipeline::locationFromString(revng::ranks::Function,
                                     Function.getHandle())) {

      if (not CommentEmitted) {
        // TODO: load from configuration when it's available.
        constexpr static uint64_t WrapCommentsAt = 80;

        auto Comments = Tokens.comments(WrapCommentsAt);
        Comments.emitCategoryComment("Functions");
      }

      Emitter.emitFunctionPrototype(Function);
      Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
      Tokens.emitNewline();
      Tokens.emitNewline();
    }
  });
}

void mlir::clift::emitDynamicModelFunctions(ptml::CTokenEmitter &Tokens,
                                            const TargetCImplementation &Target,
                                            mlir::ModuleOp Module) {
  CEmitter Emitter(Tokens, Target);

  bool CommentEmitted = false;
  Module->walk([&Tokens,
                &Emitter,
                &CommentEmitted](mlir::clift::FunctionOp Function) {
    if (pipeline::locationFromString(revng::ranks::DynamicFunction,
                                     Function.getHandle())) {
      if (not CommentEmitted) {
        // TODO: load from configuration when it's available.
        constexpr static uint64_t WrapCommentsAt = 80;

        auto Comments = Tokens.comments(WrapCommentsAt);
        Comments.emitCategoryComment("Imported Dynamic Functions");
        CommentEmitted = true;
      }

      Emitter.emitFunctionPrototype(Function);
      Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
      Tokens.emitNewline();
      Tokens.emitNewline();
    }
  });
}

void mlir::clift::emitModelSegments(ptml::CTokenEmitter &Tokens,
                                    const TargetCImplementation &Target,
                                    mlir::ModuleOp Module) {
  CEmitter Emitter(Tokens, Target);

  bool CommentEmitted = false;
  Module->walk([&Tokens,
                &Emitter,
                &CommentEmitted](mlir::clift::GlobalVariableOp Segment) {
    auto MaybeLocation = pipeline::locationFromString(revng::ranks::Segment,
                                                      Segment.getHandle());
    revng_assert(MaybeLocation.has_value());

    if (not CommentEmitted) {
      // TODO: load from configuration when it's available.
      constexpr static uint64_t WrapCommentsAt = 80;

      auto Comments = Tokens.comments(WrapCommentsAt);
      Comments.emitCategoryComment("Segments");
      CommentEmitted = true;
    }

    static constexpr auto GV = ptml::CTokenEmitter::EntityKind::GlobalVariable;
    Emitter.emitDeclaration(Segment.getType(),
                            CEmitter::DeclaratorInfo{
                              .Identifier = Segment.getName(),
                              .Location = Segment.getHandle(),
                              .Attributes = {},
                              .Kind = GV,
                              .Parameters = {} });

    Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
    Tokens.emitNewline();
    Tokens.emitNewline();
  });
}

void mlir::clift::emitSingleTypeDefinition(ptml::CTokenEmitter &Tokens,
                                           const TargetCImplementation &Target,
                                           mlir::clift::DefinedType Type,
                                           TypeEmitterConfiguration Config) {
  TypeDefinitionEmitter Emitter(Tokens, Target, Config);

  Emitter.emitTypeDefinition(Type);
  Tokens.emitNewline();
}

void mlir::clift::emitHelpers(ptml::CTokenEmitter &Tokens,
                              const TargetCImplementation &Target,
                              const std::vector<mlir::ModuleOp> &Modules) {
  // TODO: emit `#include`s

  auto Graph = TypeDependencyGraph::makeHelperGraph(Modules);

  if (not Graph.empty()) {
    // TODO: load from configuration when it's available.
    constexpr static uint64_t WrapCommentsAt = 80;

    auto Comments = Tokens.comments(WrapCommentsAt);
    Comments.emitCategoryComment("Types");
  }

  emitTypeGraph(Graph,
                Tokens,
                Target,
                TypeEmitterConfiguration{
                  .TypeToOmit = {},
                  .PrintMaximumEnumValue = false,
                  .ExplicitPadding = true,
                });

  CEmitter Emitter(Tokens, Target);

  bool CommentEmitted = false;
  std::unordered_set<std::string_view> EmittedFunctions;
  for (mlir::ModuleOp Module : Modules) {
    Module->walk([&Tokens,
                  &Emitter,
                  &CommentEmitted,
                  &EmittedFunctions](mlir::clift::FunctionOp Function) {
      if (EmittedFunctions.contains(Function.getHandle()))
        return;

      if (pipeline::locationFromString(revng::ranks::HelperFunction,
                                       Function.getHandle())) {
        if (not CommentEmitted) {
          // TODO: load from configuration when it's available.
          constexpr static uint64_t WrapCommentsAt = 80;

          auto Comments = Tokens.comments(WrapCommentsAt);
          Comments.emitCategoryComment("Functions");
          CommentEmitted = true;
        }

        Emitter.emitFunctionPrototype(Function);
        Tokens.emitPunctuator(ptml::CTokenEmitter::Punctuator::Semicolon);
        Tokens.emitNewline();
        Tokens.emitNewline();

        auto [_, Success] = EmittedFunctions.emplace(Function.getHandle());
        revng_assert(Success);
      }
    });
  }

  Tokens.emitNewline();
}
