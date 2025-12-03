//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Clift/CliftDialect.h"
#include "revng/Clift/Helpers.h"
#include "revng/CliftImportModel/ImportModel.h"
#include "revng/CliftPipes/CliftContainer.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipeline/RegisterPipe.h"
#include "revng/Pipes/FileContainer.h"
#include "revng/Pipes/Kinds.h"
#include "revng/Pipes/Ranks.h"

#include "CliftTypeLookupHelper.h"

namespace clift = mlir::clift;

static void importModelTypes(const model::Binary &Model,
                             mlir::ModuleOp Module) {
  mlir::MLIRContext *Context = Module->getContext();
  Context->loadDialect<clift::CliftDialect>();

  mlir::Location Loc = mlir::UnknownLoc::get(Context);
  auto EmitError = [&]() -> mlir::InFlightDiagnostic {
    return Context->getDiagEngine().emit(Loc, mlir::DiagnosticSeverity::Error);
  };

  llvm::SmallVector<mlir::Attribute> TypeAttrs;
  for (const auto &ModelType : Model.TypeDefinitions()) {
    auto CliftType = clift::importModelType(EmitError,
                                            *Context,
                                            *ModelType,
                                            Model);

    TypeAttrs.push_back(mlir::TypeAttr::get(CliftType));
  }

  Module->setAttr("clift.type-list", mlir::ArrayAttr::get(Context, TypeAttrs));
}

class ImportCliftTypesPipe {
public:
  static constexpr auto Name = "import-clift-types";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    return { pipeline::ContractGroup(revng::kinds::Binary,
                                      0,
                                     revng::kinds::CliftModule,
                                     1) };
  }

  void run(pipeline::ExecutionContext &EC,
           const revng::pipes::BinaryFileContainer &,
           revng::pipes::CliftContainer &CliftContainer) {
    importModelTypes(*revng::getModelFromContext(EC),
                     CliftContainer.getModule());

    EC.commitUniqueTarget(CliftContainer);
  }
};

static pipeline::RegisterPipe<ImportCliftTypesPipe> Y;

// This duplicates some of the clifter logic.
// It is not nice, but still beats pulling the entirety of clifter just for
// these snippets.
// FUTURE-WIP: @fez, I'm leaving this as is for now, including the WIP
// name, please let me know what you think during the review.
class ClifterJunior {
private:
  mlir::MLIRContext *const Context;
  mlir::ModuleOp CurrentModule;

  const model::Binary &Model;
  mlir::OpBuilder Builder;

public:
  ClifterJunior(mlir::ModuleOp Module, const model::Binary &Model) :
    Context(Module.getContext()),
    CurrentModule(Module),
    Model(Model),
    Builder(Context) {

    revng_assert(clift::hasModuleAttr(Module));
    Builder.setInsertionPointToEnd(Module.getBody());
  }

private:
  template<typename FunctionT>
  const model::TypeDefinition &getPrototype(const FunctionT &Function) {
    if (auto *Result = Function.prototype())
      return *Result;

    revng_check(Model.defaultPrototype());
    return *Model.defaultPrototype();
  }

public:
  template<typename FunctionT, typename RankT, typename... ArgsT>
  clift::FunctionOp
  emitModelFunctionDeclaration(const FunctionT &MF, const RankT &Rank) {
    // TODO: consider building a map instead of looking each type up separately.
    auto Prototype = mlir::clift::lookupCliftType(CurrentModule,
                                                  getPrototype(MF));
    revng_check(Prototype.has_value());
    auto CliftPrototype = mlir::cast<mlir::clift::FunctionType>(*Prototype);

    mlir::OpBuilder::InsertionGuard Guard(Builder);
    Builder.setInsertionPointToEnd(CurrentModule.getBody());

    // NOTE: neither debug information nor name matter for the users of this.
    auto R = Builder.create<clift::FunctionOp>(mlir::UnknownLoc::get(Context),
                                               toString(MF.key()),
                                               CliftPrototype);
    R.setHandle(pipeline::locationString(Rank, MF.key()));
    return R;
  }
};

class ImportCliftFunctionsWithoutBodiesPipe {
public:
  static constexpr auto Name = "import-clift-functions-without-bodies";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    return { pipeline::ContractGroup(revng::kinds::CliftModule,
                                     0,
                                     pipeline::InputPreservation::Preserve) };
  }

  void run(pipeline::ExecutionContext &EC,
           revng::pipes::CliftContainer &CliftContainer) {
    const model::Binary &Model = *revng::getModelFromContext(EC);
    ClifterJunior Junior(CliftContainer.getModule(), Model);

    for (const auto &ModelFunction : Model.Functions())
      Junior.emitModelFunctionDeclaration(ModelFunction,
                                          revng::ranks::Function);

    for (const auto &ModelFunction : Model.ImportedDynamicFunctions())
      Junior.emitModelFunctionDeclaration(ModelFunction,
                                          revng::ranks::DynamicFunction);

    EC.commitUniqueTarget(CliftContainer);
  }
};

static pipeline::RegisterPipe<ImportCliftFunctionsWithoutBodiesPipe> Z;
