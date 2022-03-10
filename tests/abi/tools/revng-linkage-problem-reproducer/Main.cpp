/// \file Main.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <fstream>
#include <streambuf>
#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"

#include "revng/ABI/FunctionType.h"
#include "revng/Model/Binary.h"

// #include "ABIArtifactParser.h"

namespace Options {

using namespace llvm::cl;

static OptionCategory Category("Tool Options");

constexpr const char *FnDesc = "<input file name>";
static opt<std::string> Filename(Positional, Required, desc(FnDesc), cat(Category));

constexpr const char *AtDesc = "<runtime abi artifact name>";
static opt<std::string> Artifact(Positional, Required, desc(AtDesc), cat(Category));

} // namespace Options

#include "revng/Model/ToolHelpers.h"
#include "revng/Model/Pass/Verify.h"

int main(int argc, const char *argv[]) {
  llvm::cl::HideUnrelatedOptions(Options::Category);
  llvm::cl::ParseCommandLineOptions(argc, argv);
  
  llvm::ExitOnError ExitOnError;
  auto MaybeModel = ExitOnError(ModelInModule::load(Options::Filename));

  model::verify(MaybeModel.Model);
  // MaybeModel.Model->verify(true);

  return 0;
}
