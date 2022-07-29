#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <string>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/Pipeline/Contract.h"
#include "revng/Pipeline/Target.h"
#include "revng/Pipes/FileContainer.h"
#include "revng/Pipes/FunctionStringMap.h"

namespace revng::pipes {

class ProcessCallGraphPipe {
public:
  static constexpr const auto Name = "process-call-graph";

public:
  std::array<pipeline::ContractGroup, 1> getContract() const;

public:
  void run(pipeline::Context &Context,
           const pipeline::LLVMContainer &TargetsList,
           FileContainer &OutputFile);

  void print(const pipeline::Context &Ctx,
             llvm::raw_ostream &OS,
             llvm::ArrayRef<std::string> ContainerNames) const;
};

} // namespace revng::pipes
