//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Backend/DecompilePipe.h"
#include "revng/Backend/DecompileToSingleFilePipe.h"
#include "revng/PTML/CTokenEmitter.h"
#include "revng/PTML/Constants.h"
#include "revng/Pipeline/AllRegistries.h"
#include "revng/Pipes/FileContainer.h"
#include "revng/Pipes/Kinds.h"

class EmitCAsSingleFile {
public:
  static constexpr auto Name = "emit-c-as-single-file";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup({ Contract(Decompiled,
                                      0,
                                      DecompiledToC,
                                      1,
                                      InputPreservation::Preserve) }) };
  }

  void run(pipeline::ExecutionContext &EC,
           const revng::pipes::DecompileStringMap &DecompiledFunctions,
           revng::pipes::DecompiledFileContainer &OutCFile) {

    llvm::raw_string_ostream Out = OutCFile.asStream();
    CTokenEmitter Emitter(Out, ptml::Tagging::Enabled);

    auto Tag = Emitter.initializeOpenTag(ptml::tags::Div);
    Tag.finalizeOpenTag();

    // Print includes
    Emitter.emitIncludeDirective("types-and-globals.h",
                                 "",
                                 CTokenEmitter::IncludeMode::Quote);
    Emitter.emitIncludeDirective("helpers.h",
                                 "",
                                 CTokenEmitter::IncludeMode::Quote);
    Emitter.emitNewline();

    // Copy the functions one by one.
    for (const auto &[MetaAddress, CFunction] : DecompiledFunctions) {
      Emitter.emitLiteralContent(CFunction);
      Emitter.emitNewline();
    }

    Tag.close();

    EC.commitUniqueTarget(OutCFile);
  }
};
