//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/CliftImportModel/ImportModel.h"
#include "revng/CliftPipes/CliftContainer.h"
#include "revng/CliftPipes/ImportModelInfoPipe.h"
#include "revng/Pipeline/RegisterPipe.h"

//
// Old style pipes
//

class ImportFunctionModelInfoPipe {
public:
  static constexpr auto Name = "import-function-model-info";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup({ Contract(CliftFunction,
                                      0,
                                      CliftFunction,
                                      0,
                                      InputPreservation::Preserve) }) };
  }

  void run(pipeline::ExecutionContext &EC,
           revng::pipes::CliftFunctionContainer &CliftContainer) {
    mlir::ModuleOp Module = CliftContainer.getModule();
    const model::Binary &Model = *revng::getModelFromContext(EC);

    for (const model::Function &Function :
         revng::getFunctionsAndCommit(EC, CliftContainer.name())) {
      // Note that this re-imports *every* global for *every* function, which
      // is really bad from the invalidation stand point.
      //
      // The proper solution would be to manually determine which functions
      // use which globals - and only update those BUT this problem is only
      // affecting the old pipeline (in the new one, every function is in
      // a separate module only containing its dependencies).
      //
      // As such, it's not worth fixing it at this point: we can live with
      // a bunch of unnecessary invalidations until we drop the old pipeline.
      mlir::clift::importModelInfo(Function, Model, Module);
    }
  }
};

static pipeline::RegisterPipe<ImportFunctionModelInfoPipe> X;

class ImportModelInfoPipe {
public:
  static constexpr auto Name = "import-model-info";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup({ Contract(CliftModule,
                                      0,
                                      CliftModule,
                                      0,
                                      InputPreservation::Preserve) }) };
  }

  void run(pipeline::ExecutionContext &EC,
           revng::pipes::CliftContainer &CliftContainer) {
    mlir::clift::importModelInfo(*revng::getModelFromContext(EC),
                                 CliftContainer.getModule());

    EC.commitUniqueTarget(CliftContainer);
  }
};

static pipeline::RegisterPipe<ImportModelInfoPipe> Y;

//
// New style pipes
//

namespace revng::pypeline::piperuns {

using IFMN = ImportFunctionModelInfo;
void IFMN::runOnCliftFunction(const model::Function &Function,
                              mlir::clift::FunctionOp MLIR) {
  mlir::clift::importModelInfo(Binary, MLIR->getParentOfType<mlir::ModuleOp>());
}

void ImportModelInfo::run() {
  mlir::clift::importModelInfo(Binary, TypesAndGlobals.getModule());
}

} // namespace revng::pypeline::piperuns
