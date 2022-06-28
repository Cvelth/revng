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
#include "revng/Pipes/Yield/YieldCallGraphSlicePipe.h"
#include "revng/Yield/CallGraph.h"
#include "revng/Yield/SVG.h"

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
  // Access the model
  const auto &Model = revng::pipes::getModelFromContext(Context);

  // Open the input file.
  auto MaybeInputPath = Input.path();
  revng_assert(MaybeInputPath.has_value());
  auto MaybeBuffer = llvm::MemoryBuffer::getFile(MaybeInputPath.value());
  revng_assert(MaybeBuffer);
  llvm::yaml::Input YAMLInput(**MaybeBuffer);

  // Deserialize the graph data.
  yield::CallGraph CallGraph;
  YAMLInput >> CallGraph;

  // Convert the graph to SVG.
  auto Result = yield::svg::calls(CallGraph, *Model);

  // Open the output file.
  std::error_code ErrorCode;
  llvm::raw_fd_ostream Stream(Output.getOrCreatePath(), ErrorCode);
  if (ErrorCode)
    revng_abort(ErrorCode.message().c_str());

  // Print the result.
  Stream << Result;
  Stream.flush();
  if ((ErrorCode = Stream.error()))
    revng_abort(ErrorCode.message().c_str());
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

void YieldCallGraphSlicePipe::run(pipeline::Context &Context,
                                  const pipeline::LLVMContainer &TargetList,
                                  const FileContainer &Input,
                                  FunctionStringMap &Output) {
  // Access the model
  const auto &Model = revng::pipes::getModelFromContext(Context);

  // Open the input file.
  auto MaybeInputPath = Input.path();
  revng_assert(MaybeInputPath.has_value());
  auto MaybeBuffer = llvm::MemoryBuffer::getFile(MaybeInputPath.value());
  revng_assert(MaybeBuffer);
  llvm::yaml::Input YAMLInput(**MaybeBuffer);

  // Deserialize the graph data.
  yield::CallGraph CallGraph;
  YAMLInput >> CallGraph;

  // Access the llvm module
  const llvm::Module &Module = TargetList.getModule();

  for (const auto &LLVMFunction : FunctionTags::Isolated.functions(&Module)) {
    auto Metadata = extractFunctionMetadata(&LLVMFunction);

    // Convert a slice of the graph to SVG.
    Output.insert_or_assign(Metadata->Entry,
                            yield::svg::callsSlice(Metadata->Entry,
                                                   CallGraph,
                                                   *Model));
  }
}

void YieldCallGraphSlicePipe::print(const pipeline::Context &,
                                    llvm::raw_ostream &OS,
                                    llvm::ArrayRef<std::string>) const {
  OS << *revng::ResourceFinder.findFile("bin/revng") << " magic ^_^\n";
}

std::array<pipeline::ContractGroup, 1>
YieldCallGraphSlicePipe::getContract() const {
  pipeline::Contract FunctionContract(Isolated,
                                      pipeline::Exactness::Exact,
                                      0,
                                      FunctionCallGraphSVG,
                                      2,
                                      pipeline::InputPreservation::Preserve);

  pipeline::Contract GraphContract(BinaryCallGraphInternal,
                                   pipeline::Exactness::Exact,
                                   1,
                                   pipeline::InputPreservation::Preserve);

  return { pipeline::ContractGroup{ std::move(FunctionContract),
                                    std::move(GraphContract) } };
}

static pipeline::RegisterContainerFactory
  InternalContainer("BinaryCallGraphInternal",
                    makeFileContainerFactory(BinaryCallGraphInternal,
                                             "application/"
                                             "x.yaml.call-graph.internal"));
static pipeline::RegisterContainerFactory
  BinaryGraphContainer("BinaryCallGraphSVG",
                       makeFileContainerFactory(BinaryCallGraphSVG,
                                                "application/"
                                                "x.yaml.call-graph.svg-body"));
static revng::pipes::RegisterFunctionStringMap
  FunctionGraphContainer("FunctionCallGraphSVG",
                         "application/x.yaml.call-graph-slice.svg-body",
                         revng::pipes::FunctionCallGraphSVG);

static pipeline::RegisterPipe<ProcessCallGraphPipe> ProcessPipe;
static pipeline::RegisterPipe<YieldCallGraphPipe> YieldPipe;
static pipeline::RegisterPipe<YieldCallGraphSlicePipe> YieldSlicePipe;

} // end namespace revng::pipes
