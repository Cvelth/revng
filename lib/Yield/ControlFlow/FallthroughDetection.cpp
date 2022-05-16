/// \file FallthroughDetection.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Support/MetaAddress.h"
#include "revng/Yield/ControlFlow/FallthroughDetection.h"
#include "revng/Yield/Internal/Function.h"

template<bool ShouldMergeFallthroughTargets>
const yield::BasicBlock *
yield::cfg::detectFallthrough(const yield::BasicBlock &BasicBlock,
                              const yield::Function &Function,
                              const model::Binary &Binary) {
  const yield::BasicBlock *Result = nullptr;

  for (const MetaAddress &Target : BasicBlock.Targets) {
    if (Target.isValid() && Target == BasicBlock.NextAddress) {
      auto Iterator = Function.BasicBlocks.find(Target);
      if (Iterator != Function.BasicBlocks.end()) {
        using namespace yield::BasicBlockType;
        if (shouldSkip<ShouldMergeFallthroughTargets>(Iterator->Type)) {
          revng_assert(Result == nullptr,
                       "Multiple targets with the same address");
          Result = &*Iterator;
        }
      }
    }
  }

  return Result;
}

template<bool ShouldMergeFallthroughTargets>
llvm::SmallVector<MetaAddress, 4>
yield::cfg::labeledBlock(const yield::BasicBlock &BasicBlock,
                         const yield::Function &Function,
                         const model::Binary &Binary) {
  // Blocks that are a part of another labeled block cannot start a new one.
  using namespace yield::BasicBlockType;
  if (shouldSkip<ShouldMergeFallthroughTargets>(BasicBlock.Type))
    return {};

  llvm::SmallVector<MetaAddress, 4> Result;

  constexpr bool MergeFallthrough = ShouldMergeFallthroughTargets;
  auto Next = detectFallthrough<MergeFallthrough>(BasicBlock, Function, Binary);
  while (Next != nullptr) {
    Result.emplace_back(Next->Address);
    Next = detectFallthrough<MergeFallthrough>(*Next, Function, Binary);
  }

  return Result;
}

template llvm::SmallVector<MetaAddress, 4>
yield::cfg::labeledBlock<true>(const yield::BasicBlock &BasicBlock,
                               const yield::Function &Function,
                               const model::Binary &Binary);
template llvm::SmallVector<MetaAddress, 4>
yield::cfg::labeledBlock<false>(const yield::BasicBlock &BasicBlock,
                                const yield::Function &Function,
                                const model::Binary &Binary);
