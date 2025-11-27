#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/PTML/CTokenEmitter.h"

namespace mlir {

class ModuleOp;

namespace clift {

// TODO: should this header become a class?

ptml::CTokenEmitter::Scope emitHeaderPrologue(ptml::CTokenEmitter &PTML);

struct TypeSystemEmitterConfiguration {
  /// We don't always want the complete type system. For example, when one
  /// of the types is being edited, we cannot include it, as well as every type
  /// that depends on its definition.
  ///
  /// Note: the type is identified by its handle.
  llvm::StringRef TypeToOmit = {};

  /// Because we are emitting C11, we cannot specify underlying enum type
  /// (the feature was only backported from C++ in C23), which means that
  /// when we need to preserve enum size across the compilation boundary
  /// (for example, when editing a type), we need to resolve to a trick:
  /// we print the maximum value possible - and then use it to figure out
  /// the original size.
  /// Setting this flag to true enables printing of such a value.
  bool EnablePrintingOfTheMaximumEnumValue = false;

  /// When editing types, it would be extremely annoying to have to do every
  /// change twice. As such, we need a way to disable printing of the explicit
  /// padding fields.
  ///
  /// Note that setting this leads to changes in the struct layout were they
  /// recompiled with a normal compiler (our wrapper used when editing types
  /// explicitly handles `_STARTS_AT` tags, so it's not affected.
  bool SupressExplicitPadding = false;
};

void emitModelIncludes(ptml::CTokenEmitter &PTML);
void emitModelTypes(ptml::CTokenEmitter &PTML,
                    const TargetCImplementation &Target,
                    const mlir::ModuleOp &Module,
                    TypeSystemEmitterConfiguration Configuration = {});
void emitModelFunctions(ptml::CTokenEmitter &PTML,
                        const TargetCImplementation &Target,
                        const mlir::ModuleOp &Module);
void emitDynamicModelFunctions(ptml::CTokenEmitter &PTML,
                               const TargetCImplementation &Target,
                               const mlir::ModuleOp &Module);
void emitModelSegments(ptml::CTokenEmitter &PTML,
                       const TargetCImplementation &Target,
                       const mlir::ModuleOp &Module);
inline void emitModelHeader(ptml::CTokenEmitter &PTML,
                            const TargetCImplementation &Target,
                            const mlir::ModuleOp &Module,
                            TypeSystemEmitterConfiguration Configuration = {}) {
  auto Scope = emitHeaderPrologue(PTML);

  emitModelIncludes(PTML);

  // TODO: split the following into separate headers.

  emitModelTypes(PTML, Target, Module, Configuration);
  emitModelFunctions(PTML, Target, Module);
  emitDynamicModelFunctions(PTML, Target, Module);
  emitModelSegments(PTML, Target, Module);
}

void emitHelpers(ptml::CTokenEmitter &PTML,
                 const TargetCImplementation &Target,
                 const mlir::ModuleOp &Module);
inline void emitHelperHeader(ptml::CTokenEmitter &PTML,
                             const TargetCImplementation &Target,
                             const mlir::ModuleOp &Module) {
  auto Scope = emitHeaderPrologue(PTML);

  emitHelpers(PTML, Target, Module);
}

void emitAttributes(ptml::CTokenEmitter &PTML);
inline void emitAttributeHeader(ptml::CTokenEmitter &PTML) {
  auto Scope = emitHeaderPrologue(PTML);

  emitAttributes(PTML);
}

void emitPrimitiveTypes(ptml::CTokenEmitter &PTML);
inline void emitPrimitiveHeader(ptml::CTokenEmitter &PTML) {
  auto Scope = emitHeaderPrologue(PTML);

  emitPrimitiveTypes(PTML);
}

class DefinedType;
void emitSingleTypeDefinition(ptml::CTokenEmitter &PTML,
                              const TargetCImplementation &Target,
                              const mlir::clift::DefinedType &Type,
                              TypeSystemEmitterConfiguration
                                Configuration = {});

} // namespace clift
} // namespace mlir
