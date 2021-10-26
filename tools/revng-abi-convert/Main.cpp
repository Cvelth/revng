/// \file Main.cpp
/// \brief This tool is an interface on top of `revng/StaticAnalysis/ABI` used
/// for easy abi conversion.
///
/// It works on `revng::model`. All the valid and convertible functions as
/// well as all the types they depend on from the input model are added to
/// the output model

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <fstream>
#include <streambuf>
#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"

#include "Convert.h"

namespace Options {

using namespace llvm::cl;

OptionCategory Category("ABI Options");

enum ConvertionType { ToRaw, ToCABI };

auto OpV = values(clEnumVal(ToRaw,
                            "Convert all the CABI functions to "
                            "`RawFunctionType` representation"),
                  clEnumVal(ToCABI,
                            "Convert all the Raw functions to "
                            "`CABIFunctionType` representation"));
using CT = ConvertionType;
opt<CT> Operation("op", desc("Operation"), OpV, init(ToRaw), cat(Category));

using namespace model::abi;
auto TaV = values(clEnumVal(SystemV_x86_64, "64-bit SystemV x86 abi"),
                  clEnumVal(SystemV_x86, "32-bit SystemV x86 abi"),
                  clEnumVal(SystemV_x86_regparm_3,
                            "32-bit SystemV x86 abi that allows the first "
                            "three GPR-sized primitive arguments to be passed "
                            "using the registers"),
                  clEnumVal(SystemV_x86_regparm_2,
                            "32-bit SystemV x86 abi that allows the first two "
                            "GPR-sized primitive arguments to be passed using "
                            "the registers"),
                  clEnumVal(SystemV_x86_regparm_1,
                            "32-bit SystemV x86 abi that allows the first "
                            "GPR-sized primitive argument to be passed using "
                            "the registers"),
                  clEnumVal(Microsoft_x64, "64-bit Microsoft x86 abi"),
                  clEnumVal(Microsoft_x64_vectorcall,
                            "64-bit Microsoft x86 abi with extra vector "
                            "registers designited for passing function "
                            "arguments"),
                  clEnumVal(Microsoft_x64_clrcall,
                            "64-bit Microsoft x86 abi that uses CLR expression "
                            "stack for passing function arguments"),
                  clEnumVal(Microsoft_x86_cdecl,
                            "32-bit Microsoft x86 abi that was intended to "
                            "mimic 32-bit SystemV x86 abi but has minor "
                            "differences"),
                  clEnumVal(Microsoft_x86_stdcall,
                            "32-bit Microsoft x86 abi, it is a modification of "
                            "`stdcall` that's different in a sense that the "
                            "callee is responsible for stack cleanup instead "
                            "of the caller"),
                  clEnumVal(Microsoft_x86_thiscall,
                            "32-bit Microsoft x86 abi, it extends `stdcall` by "
                            "allowing `this` pointer in method-style calls to "
                            "be passed using a register. It is never used for "
                            "'free' functions"),
                  clEnumVal(Microsoft_x86_fastcall,
                            "32-bit Microsoft x86 abi, it extends `stdcall` by "
                            "allowing two first GRP-sized function arguments "
                            "to be passed using the registers"),
                  clEnumVal(Microsoft_x86_clrcall,
                            "32-bit Microsoft x86 abi that uses CLR expression "
                            "stack for function argument passing"),
                  clEnumVal(Microsoft_x86_vectorcall,
                            "64-bit Microsoft x86 abi, it extends `fastcall` "
                            "by allowing extra vector registers to be used for "
                            "function argument passing"));

using ABI = model::abi::Values;
constexpr const char *TrgDesc = "The ABI of the input/output binaries.";
opt<ABI> TargetABI("abi", Required, desc(TrgDesc), TaV, cat(Category));

constexpr const char *FnDesc = "<input file name>";
opt<std::string> Filename(Positional, Required, desc(FnDesc), cat(Category));

opt<std::string> Output("o",
                        desc("Optional output filename, if not specified, the "
                             "output is dumped to `stdout`"),
                        value_desc("path"),
                        cat(Category));

} // namespace Options

int main(int argc, const char *argv[]) {
  // Enable LLVM stack trace
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  llvm::cl::HideUnrelatedOptions(Options::Category);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  llvm::raw_fd_ostream *OutputStreamPtr;
  if (Options::Output.empty()) {
    OutputStreamPtr = &llvm::outs();
  } else {
    std::error_code EC;
    static llvm::raw_fd_ostream OutputStream(Options::Output, EC);
    if (!EC || OutputStream.has_error()) {
      llvm::errs() << "Unable to open an output file: '" << Options::Output
                   << "'.\n";
      return 1;
    }
    OutputStreamPtr = &OutputStream;
  }

  auto BufferOrError = llvm::MemoryBuffer::getFileOrSTDIN(Options::Filename);
  if (!BufferOrError) {
    llvm::errs() << "Unable to open an input file: '" << Options::Filename
                 << "'.\n";
    return 2;
  }
  llvm::StringRef InputText = (*BufferOrError)->getBuffer();

  auto Deserialized = TupleTree<model::Binary>::deserialize(InputText);
  if (!Deserialized) {
    llvm::errs() << "Unable to deserialize the model: '" << Options::Filename
                 << "'.\n";
    return 3;
  }
  if (!Deserialized->verify()) {
    llvm::errs() << "Model verification failed: '" << Options::Filename
                 << "'.\n";
    return 4;
  }

  if (Options::Operation == Options::ToCABI)
    return convertToCABI(*Deserialized, Options::TargetABI, *OutputStreamPtr);
  else
    return convertToRaw(*Deserialized, Options::TargetABI, *OutputStreamPtr);
}
