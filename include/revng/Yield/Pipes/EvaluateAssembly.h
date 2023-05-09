#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <string>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/Pipeline/Contract.h"
#include "revng/Pipes/FunctionStringMap.h"
#include "revng/Pipes/StringBufferContainer.h"
#include "revng/Yield/Pipes/YieldControlFlow.h"

namespace revng::pipes {

inline constexpr char AssemblyEvaluationName[] = "AssemblyEvaluation";
inline constexpr char AssemblyEvaluationMIMEType[] = "text/x.S";
inline constexpr char AssemblyEvaluationSuffix[] = ".S";
using AssemblyEvaluationFileContainer = StringBufferContainer<
  &kinds::AssemblyEvaluation,
  AssemblyEvaluationName,
  AssemblyEvaluationMIMEType,
  AssemblyEvaluationSuffix>;

class EvaluateAssembly {
public:
  static constexpr const auto Name = "EvaluateAssembly";

public:
  inline std::array<pipeline::ContractGroup, 1> getContract() const {
    return { pipeline::ContractGroup(kinds::FunctionAssemblyInternal,
                                     0,
                                     kinds::AssemblyEvaluation,
                                     1,
                                     pipeline::InputPreservation::Preserve) };
  }

public:
  void run(pipeline::Context &Context,
           const FunctionAssemblyStringMap &Input,
           AssemblyEvaluationFileContainer &Output);

  void print(const pipeline::Context &Ctx,
             llvm::raw_ostream &OS,
             llvm::ArrayRef<std::string> ContainerNames) const;
};

} // namespace revng::pipes
