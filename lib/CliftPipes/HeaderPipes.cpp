//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LogicalResult.h"

#include "revng/Clift/CliftTypeInterfaces.h"
#include "revng/CliftEmitC/CEmitter.h"
#include "revng/CliftEmitC/CSemantics.h"
#include "revng/CliftEmitC/Headers.h"
#include "revng/CliftPipes/CliftContainer.h"
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

} // namespace

//
// New style pipes
//

namespace revng::pypeline::piperuns {

// TODO

} // namespace revng::pypeline::piperuns
