//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/EarlyFunctionAnalysis/FunctionMetadata.h"
#include "revng/EarlyFunctionAnalysis/IRHelpers.h"
#include "revng/Model/Binary.h"
#include "revng/Pipeline/Pipe.h"
#include "revng/Pipeline/RegisterContainerFactory.h"
#include "revng/Pipeline/RegisterPipe.h"
#include "revng/Pipes/ModelGlobal.h"
#include "revng/Pipes/Yield/Kinds.h"
#include "revng/Pipes/Yield/ProcessCallGraphPipe.h"
#include "revng/Pipes/Yield/YieldCallGraphPipe.h"
#include "revng/Yield/CallGraph.h"

namespace revng::pipes {

void ProcessCallGraphPipe::run(pipeline::Context &Context,
                               const pipeline::LLVMContainer &TargetList,
                               FileContainer &OutputFile) {
  // Access the model
  const auto &Model = revng::pipes::getModelFromContext(Context);

  // Access the llvm module
  const llvm::Module &Module = TargetList.getModule();

  // Gather function metadata
  SortedVector<efa::FunctionMetadata> Metadata;
  for (const auto &LLVMFunction : FunctionTags::Isolated.functions(&Module))
    Metadata.insert(*extractFunctionMetadata(&LLVMFunction));
  revng_assert(Metadata.size() == Model->Functions.size());

  // Collect the call graph data.
  yield::CallGraph CallGraph(Metadata, *Model);

  // Serialize the graph data.
  std::error_code ErrorCode;
  llvm::raw_fd_ostream Stream(OutputFile.getOrCreatePath(), ErrorCode);
  if (ErrorCode)
    revng_abort(ErrorCode.message().c_str());

  llvm::yaml::Output YAMLOutput(Stream);
  YAMLOutput << CallGraph;

  Stream.flush();
  if ((ErrorCode = Stream.error()))
    revng_abort(ErrorCode.message().c_str());
}

void ProcessCallGraphPipe::print(const pipeline::Context &,
                                 llvm::raw_ostream &OS,
                                 llvm::ArrayRef<std::string>) const {
  OS << *revng::ResourceFinder.findFile("bin/revng") << " magic ^_^\n";
}

std::array<pipeline::ContractGroup, 1>
ProcessCallGraphPipe::getContract() const {
  return { pipeline::ContractGroup(IsolatedRoot,
                                   pipeline::Exactness::Exact,
                                   0,
                                   BinaryCallGraphInternal,
                                   1,
                                   pipeline::InputPreservation::Preserve) };
}

void YieldCallGraphPipe::run(pipeline::Context &Context,
                             const FileContainer &Input,
                             FileContainer &Output) {
  revng_abort("WIP");
}

void YieldCallGraphPipe::print(const pipeline::Context &,
                               llvm::raw_ostream &OS,
                               llvm::ArrayRef<std::string>) const {
  OS << *revng::ResourceFinder.findFile("bin/revng") << " magic ^_^\n";
}

std::array<pipeline::ContractGroup, 1> YieldCallGraphPipe::getContract() const {
  return { pipeline::ContractGroup(BinaryCallGraphInternal,
                                   pipeline::Exactness::Exact,
                                   0,
                                   BinaryCallGraphSVG,
                                   1,
                                   pipeline::InputPreservation::Preserve) };
}

static pipeline::RegisterContainerFactory
  InternalContainer("BinaryCallGraphInternal",
                    makeFileContainerFactory(BinaryCallGraphInternal,
                                             "application/"
                                             "x.yaml.call-graph.internal"));
static pipeline::RegisterContainerFactory
  SVGGraphContainer("BinaryCallGraphSVG",
                    makeFileContainerFactory(BinaryCallGraphSVG,
                                             "application/"
                                             "x.yaml.call-graph.svg-body"));

static pipeline::RegisterPipe<ProcessCallGraphPipe> ProcessPipe;
static pipeline::RegisterPipe<YieldCallGraphPipe> YieldPipe;

} // end namespace revng::pipes
