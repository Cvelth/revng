/// \file Main.cpp
/// \brief This tool is an interface on top of `TupleTreeDiff` used for easy
/// comparison between two serialized models. It returns 0 if models are
/// equivalent and 255 if they are not. Other error codes like 65 or 128 (when
/// opening one of the files fails) could also be returned alongside an error
/// message to `stderr`.

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <iostream>
#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Signals.h"

#include "revng/Model/Binary.h"
#include "revng/Model/TupleTreeDiff.h"

namespace Options {

using namespace llvm::cl;

OptionCategory Category("Comparator Options");

opt<std::string> FirstFile(Positional,
                           Required,
                           llvm::cl::desc("<first file>"),
                           cat(Category));
opt<std::string> SecondFile(Positional,
                            Optional,
                            llvm::cl::desc("<second file>"),
                            cat(Category));

} // namespace Options

int main(int argc, const char *argv[]) {
  // Enable LLVM stack trace
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  llvm::cl::HideUnrelatedOptions(Options::Category);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  auto FirstBuffer = llvm::MemoryBuffer::getFileOrSTDIN(Options::FirstFile);
  if (!FirstBuffer) {
    dbg << "Unable to open a file: '" << Options::FirstFile << "'.\n";
    return 65;
  }
  llvm::StringRef FirstYml = (*FirstBuffer)->getBuffer();
  auto FirstDeserialized = TupleTree<model::Binary>::deserialize(FirstYml);
  if (!FirstDeserialized) {
    dbg << "Unable to deserialize the model: '" << Options::FirstFile << "'.\n";
    return 66;
  }
  if (!FirstDeserialized->verify()) {
    dbg << "Model verification failed: '" << Options::FirstFile << "'.\n";
    return 67;
  }
  model::Binary &FirstModel = **FirstDeserialized;

  std::string_view InputFileName = "-";
  if (Options::SecondFile.hasArgStr())
    InputFileName = Options::SecondFile;
  auto SecondBuffer = llvm::MemoryBuffer::getFileOrSTDIN(InputFileName.data());
  if (!SecondBuffer) {
    dbg << "Unable to open a file: '" << Options::SecondFile << "'.\n";
    return 128;
  }
  llvm::StringRef SecondYml = (*SecondBuffer)->getBuffer();
  auto SecondDeserialized = TupleTree<model::Binary>::deserialize(SecondYml);
  if (!SecondDeserialized) {
    dbg << "Unable to deserialize the model: '" << Options::SecondFile
        << "'.\n";
    return 129;
  }
  if (!SecondDeserialized->verify()) {
    dbg << "Model verification failed: '" << Options::SecondFile << "'.\n";
    return 130;
  }
  model::Binary &SecondModel = **SecondDeserialized;

  // todo: use `diff(model::Binary &, model::Binary &)::dump` for output
  // generation after it's back in working order.
  if (!diff(FirstModel, SecondModel).Changes.empty()) {
    dbg << "Models are not equivalent: \n"
        << std::string_view(FirstYml) << "\nand\n"
        << std::string_view(SecondYml) << "\n\n";
    return 255;
  }

  return 0;
}
