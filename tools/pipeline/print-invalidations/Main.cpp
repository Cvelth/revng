// \file Main.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <system_error>

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_os_ostream.h"

#include "revng/Model/LoadModelPass.h"
#include "revng/Pipeline/AllRegistries.h"
#include "revng/Pipeline/ContainerSet.h"
#include "revng/Pipeline/CopyPipe.h"
#include "revng/Pipeline/GenericLLVMPipe.h"
#include "revng/Pipeline/Global.h"
#include "revng/Pipeline/LLVMContainerFactory.h"
#include "revng/Pipeline/LLVMGlobalKindBase.h"
#include "revng/Pipeline/Loader.h"
#include "revng/Pipeline/Step.h"
#include "revng/Pipeline/Target.h"
#include "revng/Pipes/ModelGlobal.h"
#include "revng/Pipes/PipelineManager.h"
#include "revng/Pipes/ToolCLOptions.h"
#include "revng/Support/CommandLine.h"
#include "revng/Support/InitRevng.h"
#include "revng/TupleTree/TupleTree.h"

using std::string;
using namespace llvm;
using namespace llvm::cl;
using namespace pipeline;
using namespace ::revng::pipes;
using namespace revng;

static cl::opt<std::string> DiffPath(cl::Positional,
                                     cl::cat(MainCategory),
                                     cl::desc("<model diff>"),
                                     cl::init("-"),
                                     cl::value_desc("model"));

static ToolCLOptions BaseOptions(MainCategory);

static ExitOnError AbortOnError;

int main(int argc, const char *argv[]) {
  using BinaryRef = TupleTreeGlobal<model::Binary>;
  using DiffRef = TupleTreeDiff<model::Binary>;

  revng::InitRevng X(argc, argv);

  HideUnrelatedOptions(MainCategory);
  ParseCommandLineOptions(argc, argv);

  Registry::runAllInitializationRoutines();

  auto Manager = AbortOnError(BaseOptions.makeManager());
  const auto &Ctx = Manager.context();
  auto OriginalModel = *AbortOnError(Ctx.getGlobal<BinaryRef>(ModelGlobalName));

  const auto &Name = ModelGlobalName;
  auto AfterModel = AbortOnError(Ctx.getGlobal<BinaryRef>(Name));

  auto Diff = AbortOnError(deserializeFileOrSTDIN<DiffRef>(DiffPath));
  Diff.apply(AfterModel->get());

  TargetsList OverestimatedTargets;
  for (const Kind &Rule : Manager.getRunner().getKindsRegistry())
    Rule.getInvalidations(Manager.context(),
                          OverestimatedTargets,
                          Diff,
                          OriginalModel,
                          *AfterModel);

  OverestimatedTargets.dump(llvm::outs());

  return EXIT_SUCCESS;
}
