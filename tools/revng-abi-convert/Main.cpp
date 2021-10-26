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

#define clABIDescription(name) clEnumVal(name, getABIDescription(name))
inline constexpr std::string_view getABIDescription(model::abi::Values V) {
  switch (V) {
  case model::abi::SystemV_x86_64:
    return "64-bit SystemV x86 abi";
  case model::abi::Microsoft_x64:
    return "64-bit Microsoft x86 abi";
  case model::abi::Microsoft_x64_clrcall:
    return "64-bit Microsoft x86 abi that uses CLR expression "
           "stack for passing function arguments";
  case model::abi::Microsoft_x64_vectorcall:
    return "64-bit Microsoft x86 abi with extra vector "
           "registers designited for passing function "
           "arguments";

  case model::abi::SystemV_x86:
    return "32-bit SystemV x86 abi";
  case model::abi::SystemV_x86_regparm_3:
    return "32-bit SystemV x86 abi that allows the first three GPR-sized "
           "primitive arguments to be passed using the registers";
  case model::abi::SystemV_x86_regparm_2:
    return "32-bit SystemV x86 abi that allows the first two "
           "GPR-sized primitive arguments to be passed using "
           "the registers";
  case model::abi::SystemV_x86_regparm_1:
    return "32-bit SystemV x86 abi that allows the first "
           "GPR-sized primitive argument to be passed using "
           "the registers";
  case model::abi::Microsoft_x86_clrcall:
    return "32-bit Microsoft x86 abi that uses CLR expression "
           "stack for function argument passing";
  case model::abi::Microsoft_x86_vectorcall:
    return "64-bit Microsoft x86 abi, it extends `fastcall` "
           "by allowing extra vector registers to be used for "
           "function argument passing";
  case model::abi::Microsoft_x86_cdecl:
    return "32-bit Microsoft x86 abi that was intended to "
           "mimic 32-bit SystemV x86 abi but has minor "
           "differences";
  case model::abi::Microsoft_x86_stdcall:
    return "32-bit Microsoft x86 abi, it is a modification of "
           "`stdcall` that's different in a sense that the "
           "callee is responsible for stack cleanup instead "
           "of the caller";
  case model::abi::Microsoft_x86_fastcall:
    return "32-bit Microsoft x86 abi, it extends `stdcall` by "
           "allowing two first GRP-sized function arguments "
           "to be passed using the registers";
  case model::abi::Microsoft_x86_thiscall:
    return "32-bit Microsoft x86 abi, it extends `stdcall` by "
           "allowing `this` pointer in method-style calls to "
           "be passed using a register. It is never used for "
           "'free' functions";

  case model::abi::Count:
  case model::abi::Invalid:
  default:
    return "Unknown and/or unsupported ABI";
  }
}

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
using ABI = model::abi::Values;
constexpr const char *TrgDesc = "The ABI of the input/output binaries.";
auto TaV = values(clABIDescription(SystemV_x86_64),
                  clABIDescription(SystemV_x86),
                  clABIDescription(SystemV_x86_regparm_3),
                  clABIDescription(SystemV_x86_regparm_2),
                  clABIDescription(SystemV_x86_regparm_1),
                  clABIDescription(Microsoft_x64),
                  clABIDescription(Microsoft_x64_vectorcall),
                  clABIDescription(Microsoft_x64_clrcall),
                  clABIDescription(Microsoft_x86_cdecl),
                  clABIDescription(Microsoft_x86_stdcall),
                  clABIDescription(Microsoft_x86_thiscall),
                  clABIDescription(Microsoft_x86_fastcall),
                  clABIDescription(Microsoft_x86_clrcall),
                  clABIDescription(Microsoft_x86_vectorcall));
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
      dbg << "Unable to open an output file: '" << Options::Output << "'.\n";
      return 1;
    }
    OutputStreamPtr = &OutputStream;
  }

  auto BufferOrError = llvm::MemoryBuffer::getFileOrSTDIN(Options::Filename);
  if (!BufferOrError) {
    dbg << "Unable to open an input file: '" << Options::Filename << "'.\n";
    return 2;
  }
  llvm::StringRef InputText = (*BufferOrError)->getBuffer();

  auto Deserialized = TupleTree<model::Binary>::deserialize(InputText);
  if (!Deserialized) {
    dbg << "Unable to deserialize the model: '" << Options::Filename << "'.\n";
    return 3;
  }
  if (!Deserialized->verify()) {
    dbg << "Model verification failed: '" << Options::Filename << "'.\n";
    return 4;
  }

  if (Options::Operation == Options::ToCABI)
    return convertToCABI(*Deserialized, Options::TargetABI, *OutputStreamPtr);
  else
    return convertToRaw(*Deserialized, Options::TargetABI, *OutputStreamPtr);
}
