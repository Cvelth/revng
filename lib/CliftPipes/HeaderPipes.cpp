//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Support/LogicalResult.h"

#include "revng/Clift/CliftTypeInterfaces.h"
#include "revng/CliftEmitC/CEmitter.h"
#include "revng/CliftEmitC/CSemantics.h"
#include "revng/CliftEmitC/Headers.h"
#include "revng/CliftImportModel/ImportModel.h"
#include "revng/CliftPipes/CliftContainer.h"
#include "revng/CliftPipes/HeaderPipes.h"
#include "revng/PTML/CTokenEmitter.h"
#include "revng/Pipeline/RegisterPipe.h"

#include "HeaderContainers.h"

//
// Shared logic
//

static void emitModelHeaderImpl(llvm::raw_ostream &Out, mlir::ModuleOp Module) {
  mlir::clift::TypeEmitterConfiguration Configuration = {
    .TypeToOmit = {},
    .PrintMaximumEnumValue = false,
    .ExplicitPadding = true,
  };

  ptml::CTokenEmitter Tokens(Out, ptml::Tagging::Enabled);

  // TODO: select target properly
  const auto &Target = TargetCImplementation::Default;
  mlir::clift::emitModelHeader(Tokens, Target, Module, Configuration);

  Out.flush();
}

static void emitHelperHeaderImpl(llvm::raw_ostream &Out,
                                 const std::vector<mlir::ModuleOp> &Modules) {
  ptml::CTokenEmitter Tokens(Out, ptml::Tagging::Enabled);

  // TODO: select target properly
  const auto &Target = TargetCImplementation::Default;
  mlir::clift::emitHelperHeader(Tokens, Target, Modules);

  Out.flush();
}

static void emitTypeDefinitionImpl(llvm::raw_ostream &Out,
                                   mlir::MLIRContext &Context,
                                   const model::TypeDefinition &Type) {
  ptml::CTokenEmitter Tokens(Out, ptml::Tagging::Enabled);

  auto EmitError = [&Context]() -> mlir::InFlightDiagnostic {
    return Context.getDiagEngine().emit(mlir::UnknownLoc::get(&Context),
                                        mlir::DiagnosticSeverity::Error);
  };
  auto CliftType = mlir::clift::importModelType(EmitError, Context, Type);
  revng_check(CliftType != nullptr);

  mlir::clift::TypeEmitterConfiguration Configuration = {
    .TypeToOmit = {},
    .PrintMaximumEnumValue = true,
    .ExplicitPadding = false,
  };

  // TODO: select target properly
  const auto &Target = TargetCImplementation::Default;
  emitSingleTypeDefinition(Tokens, Target, CliftType, Configuration);

  Out.flush();
}

//
// Old style pipes
//

namespace {

class ModelHeaderPipe {
public:
  static constexpr auto Name = "emit-model-header";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup({ Contract(CliftModule,
                                      0,
                                      ModelHeader,
                                      1,
                                      InputPreservation::Preserve) }) };
  }

  void run(pipeline::ExecutionContext &EC,
           const revng::pipes::CliftContainer &CliftContainer,
           ModelHeaderContainer &HeaderFile) {
    llvm::raw_string_ostream Stream = HeaderFile.asStream();
    emitModelHeaderImpl(Stream, CliftContainer.getModule());
    EC.commitUniqueTarget(HeaderFile);
  }
};

static pipeline::RegisterPipe<ModelHeaderPipe> ModelHeader;

class HelperHeaderPipe {
public:
  static constexpr auto Name = "emit-helper-header";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup({ Contract(CliftFunction,
                                      0,
                                      HelperHeader,
                                      1,
                                      InputPreservation::Preserve) }) };
  }

  void run(pipeline::ExecutionContext &EC,
           const revng::pipes::CliftContainer &CliftContainer,
           HelperHeaderContainer &HeaderFile) {
    llvm::raw_string_ostream Stream = HeaderFile.asStream();
    emitHelperHeaderImpl(Stream, { CliftContainer.getModule() });
    EC.commitUniqueTarget(HeaderFile);
  }
};

static pipeline::RegisterPipe<HelperHeaderPipe> HelperHeader;

class EmitTypeDefinition {
public:
  static constexpr auto Name = "emit-type-definition";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup({ Contract(CliftModule,
                                      0,
                                      ModelTypeDefinition,
                                      1,
                                      InputPreservation::Preserve) }) };
  }

  void run(pipeline::ExecutionContext &EC,
           const revng::pipes::CliftContainer &CliftContainer,
           TypeDefinitionContainer &ModelTypesContainer) {
    for (const model::TypeDefinition &Type :
         revng::getTypeDefinitionsAndCommit(EC, ModelTypesContainer.name())) {
      std::string &Result = ModelTypesContainer[Type.key()];
      llvm::raw_string_ostream Out(Result);
      emitTypeDefinitionImpl(Out, *CliftContainer.getContext(), Type);
    }
  }
};

static pipeline::RegisterPipe<EmitTypeDefinition> TypeDefinition;

} // namespace

//
// New style pipes
//

namespace revng::pypeline::piperuns {

void EmitModelHeader::run() {
  std::unique_ptr<llvm::raw_ostream> Out = Output.getOStream(ObjectID());
  emitModelHeaderImpl(*Out, Input.getModule());
}

void EmitHelperHeader::run() {
  std::unique_ptr<llvm::raw_ostream> Out = Output.getOStream(ObjectID());

  std::vector<mlir::ModuleOp> FunctionModules;
  for (const auto &Object : Input.objects())
    FunctionModules.emplace_back(Input.getModule(Object));

  emitHelperHeaderImpl(*Out, FunctionModules);
}

using ETD = EmitTypeDefinition;
void ETD::runOnTypeDefinition(const model::UpcastableTypeDefinition &Type) {
  revng_assert(Type);
  auto Stream = Output.getOStream(ObjectID(Type->key()));
  emitTypeDefinitionImpl(*Stream,

                         // FUTURE-WIP: this is what we get for trying to mutate
                         // an input container! cc: @fez
                         const_cast<mlir::MLIRContext &>(Input.getContext()),
                         *Type);
}

} // namespace revng::pypeline::piperuns
