#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "mlir/IR/BuiltinOps.h"

#include "revng/CliftEmitC/Configuration.h"
#include "revng/PTML/CTokenEmitter.h"

namespace mlir {

namespace clift {

void emitHeaderPrologue(ptml::CTokenEmitter &Tokens);

void emitModelIncludes(ptml::CTokenEmitter &Tokens);
void emitModelTypes(ptml::CTokenEmitter &Tokens,
                    const TargetCImplementation &Target,
                    mlir::ModuleOp Module,
                    TypeEmitterConfiguration Configuration);
void emitModelFunctions(ptml::CTokenEmitter &Tokens,
                        const TargetCImplementation &Target,
                        mlir::ModuleOp Module);
void emitDynamicModelFunctions(ptml::CTokenEmitter &Tokens,
                               const TargetCImplementation &Target,
                               mlir::ModuleOp Module);
void emitModelSegments(ptml::CTokenEmitter &Tokens,
                       const TargetCImplementation &Target,
                       mlir::ModuleOp Module);
inline void emitModelHeader(ptml::CTokenEmitter &Tokens,
                            const TargetCImplementation &Target,
                            mlir::ModuleOp Module,
                            TypeEmitterConfiguration Configuration) {
  // TODO: emit header location definition on the scope tag so that ctrl+click
  //       on includes (references) leads to this file.
  ptml::CTokenEmitter::Scope
    Scope = Tokens.enterScope(ptml::CTokenEmitter::ScopeKind::Basic, 0);

  emitHeaderPrologue(Tokens);

  emitModelIncludes(Tokens);

  // TODO: split the following into separate headers.

  emitModelTypes(Tokens, Target, Module, Configuration);
  emitModelFunctions(Tokens, Target, Module);
  emitDynamicModelFunctions(Tokens, Target, Module);
  emitModelSegments(Tokens, Target, Module);
}

class DefinedType;
void emitSingleTypeDefinition(ptml::CTokenEmitter &Tokens,
                              const TargetCImplementation &Target,
                              mlir::clift::DefinedType Type,
                              TypeEmitterConfiguration Configuration = {});

void emitHelpers(ptml::CTokenEmitter &Tokens,
                 const TargetCImplementation &Target,
                 const std::vector<mlir::ModuleOp> &Modules);
inline void emitHelperHeader(ptml::CTokenEmitter &Tokens,
                             const TargetCImplementation &Target,
                             const std::vector<mlir::ModuleOp> &Modules) {
  // TODO: emit header location definition on the scope tag so that ctrl+click
  //       on includes (references) leads to this file.
  ptml::CTokenEmitter::Scope
    Scope = Tokens.enterScope(ptml::CTokenEmitter::ScopeKind::Basic, 0);

  emitHeaderPrologue(Tokens);
  emitHelpers(Tokens, Target, Modules);
}

} // namespace clift
} // namespace mlir
