/// \file EFA4.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//
#if 0
#include <cstdint>
#include <iterator>

#include "llvm/ADT/BitVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"

#include "revng/ADT/GenericGraph.h"
#include "revng/MFP/MFP.h"
#include "revng/Model/Generated/Early/Register.h"
#include "revng/Model/Register.h"

using namespace llvm;

namespace efa {

namespace OperationType {
enum Values : uint8_t {
  Invalid,
  Read,
  Write,
  Clobber
};
} // namespace OperationType

class Operation {
public:
  OperationType::Values Type = OperationType::Invalid;
  uint8_t Target = model::Register::Invalid;
};

static_assert(sizeof(Operation) == 2);

struct Block {
public:
  using OperationsVector = SmallVector<Operation, 8>;
  using iterator = OperationsVector::iterator;

public:
  OperationsVector Operations;

public:
  Block() = default;

public:
  auto begin() const { return Operations.begin(); }
  auto end() const { return Operations.end(); }

  auto begin() { return Operations.begin(); }
  auto end() { return Operations.end(); }

  auto rbegin() const { return Operations.rbegin(); }
  auto rend() const { return Operations.rend(); }

  auto rbegin() { return Operations.rbegin(); }
  auto rend() { return Operations.rend(); }
};

using BlockNode = BidirectionalNode<Block>;

std::pair<OperationType::Values, model::Register::Values>
processInstruction(llvm::Instruction &I);

// WIP: getExitNode
// WIP: implement DOTGraphTraits
struct Function : GenericGraph<BlockNode> {
private:
  DenseMap<uint8_t, model::Register::Values> IndexToRegister;
  uint8_t RegistersCount = 0;

public:
  Function() = default;

  static Function fromLLVMFunction(llvm::Function &F) {
    using namespace model;

    Function Result;
    DenseMap<llvm::BasicBlock *, BlockNode *> BlocksMap;

    DenseMap<model::Register::Values, uint8_t> RegisterToIndex;
    auto GetIndex = [&RegisterToIndex,
                     &Result](model::Register::Values Register) -> uint8_t {
      auto It = RegisterToIndex.find(Register);
      if (It != RegisterToIndex.end())
        return It->second;

      RegisterToIndex[Register] = Result.RegistersCount;
      Result.IndexToRegister[Result.RegistersCount] = Register;

      ++Result.RegistersCount;

      return Result.RegistersCount - 1;
    };

    // Create nodes
    for (llvm::BasicBlock &BB : F) {
      auto *NewNode = Result.addNode();
      BlocksMap[&BB] = NewNode;

      // Process instructions
      for (llvm::Instruction &I : BB) {
        auto [OperationType, Register] = processInstruction(I);
        if (OperationType != OperationType::Invalid)
          NewNode->Operations.emplace_back(OperationType, GetIndex(Register));
      }
    }

    // Create edges
    for (llvm::BasicBlock &BB : F)
      for (llvm::BasicBlock *Successor : succ_range(BB))
        BlocksMap[&BB]->addSuccessor(BlocksMap[Successor]);

    return Result;
  }

public:
  model::Register::Values registerByIndex(uint8_t Index) const {
    auto It = IndexToRegister.find(Index);
    revng_assert(It != IndexToRegister.end());
    return It->second;
  }

  uint8_t registersCount() const { return RegistersCount; }

public:
  // WIP: do we actually need this?
  BlockNode &splitAt(BlockNode &Original, BlockNode::iterator SplitBefore) {
    BlockNode *NewNode = addNode();

    // Copy successor of the original node into the new node
    for (BlockNode *Successor : Original.successors())
      NewNode->addSuccessor(Successor);

    // Clear the successors of the original node
    Original.clearSuccessors();

    // Add the new node as a successor of the original node
    Original.addSuccessor(NewNode);

    // Move instructions over from the original node to the new node
    std::copy(SplitBefore,
              Original.end(),
              std::back_inserter(NewNode->Operations));
    Original.Operations.erase(SplitBefore, Original.end());

    return *NewNode;
  }
};

class LivenessAnalysis {
private:
  using Set = BitVector;
  using RegisterSet = Set;

public:
  using LatticeElement = Set;
  using GraphType = Inverse<const BlockNode *>;
  using Label = const BlockNode *;

public:
  Set combineValues(const Set &LHS, const Set &RHS) const {
    Set Result = LHS;
    Result |= RHS;
    return Result;
  }

  bool isLessOrEqual(const Set &LHS, const Set &RHS) const {
    // RHS must contain or be equal to LHS
    Set Intersection = RHS;
    Intersection &= LHS;
    return Intersection == LHS;
  }

  RegisterSet applyTransferFunction(const BlockNode *Block,
                                    const RegisterSet &InitialState) const {
    RegisterSet Result = InitialState;

    for (const Operation &Operation :
         make_range(Block->rbegin(), Block->rend())) {

      switch (Operation.Type) {
      case OperationType::Read:
        Result.set(Operation.Target);
        break;

      case OperationType::Write:
      case OperationType::Clobber:
        Result.reset(Operation.Target);
        break;

      case OperationType::Invalid:
        revng_abort();
        break;
      }
    }

    return Result;
  }
};

static_assert(MFP::MonotoneFrameworkInstance<LivenessAnalysis>);

class ReachingDefinitions {
private:
  DenseMap<const Operation *, uint8_t> WriteToIndex;

public:
  ReachingDefinitions(efa::Function &F) {
    // Populate WriteToIndex
    SmallVector<int> RegisterWriteIndex(F.registersCount(), 0);
    for (Block *Block : F.nodes()) {
      for (Operation &Operation : Block->Operations) {
        WriteToIndex[&Operation] = RegisterWriteIndex[Operation.Target];
        RegisterWriteIndex[Operation.Target] += 1;
      }
    }
  }

public:
  struct RegisterWriters {
    BitVector Reaching;
    BitVector Read;

    bool operator==(const RegisterWriters &Other) const = default;

    RegisterWriters &operator|=(const RegisterWriters &Other) {
      Reaching |= Other.Reaching;
      Read |= Other.Read;
      return *this;
    }

    RegisterWriters &operator&=(const RegisterWriters &Other) {
      Reaching &= Other.Reaching;
      Read &= Other.Read;
      return *this;
    }
  };

  using WritersSet = SmallVector<RegisterWriters, 16>;
  using LatticeElement = WritersSet;
  using GraphType = BlockNode *;
  using Label = BlockNode *;

public:
  WritersSet combineValues(const WritersSet &LHS, const WritersSet &RHS) const {
    WritersSet Result = LHS;
    for (const auto &[ResultEntry, RHSEntry] : zip(Result, RHS))
      ResultEntry |= RHSEntry;
    return Result;
  }

  bool isLessOrEqual(const WritersSet &LHS, const WritersSet &RHS) const {
    for (const auto &[LHSEntry, RHSEntry] : zip(LHS, RHS)) {
      auto Intersection = LHSEntry;
      Intersection &= RHSEntry;
      if (Intersection != LHSEntry)
        return false;
    }

    return true;
  }

  WritersSet applyTransferFunction(const Block *Block,
                                   const WritersSet &InitialState) const {
    WritersSet Result = InitialState;

    for (const Operation &Operation : *Block) {

      switch (Operation.Type) {
      case OperationType::Write:
      case OperationType::Clobber: {
        RegisterWriters &Writes = Result[Operation.Target];

        Writes.Reaching.clear();

        if (Operation.Type == OperationType::Write)
          Writes.Reaching.set(WriteToIndex.find(&Operation)->second);

      } break;
      case OperationType::Read: {
        RegisterWriters &Writes = Result[Operation.Target];
        Writes.Read |= Writes.Reaching;
      } break;
      case OperationType::Invalid:
        revng_abort();
        break;
      }
    }

    return Result;
  }
};

static_assert(MFP::MonotoneFrameworkInstance<ReachingDefinitions>);

} // namespace efa

static efa::Function
createSingleNode(efa::Block::OperationsVector &&Operations) {
  efa::Function F;
  auto *Entry = F.addNode();
  Entry->Operations = Operations;
  return F;
}

struct TestAnalysisResult {
  efa::Function Function;
  efa::BlockNode *Entry = nullptr;
  efa::BlockNode *Exit = nullptr;
};

static TestAnalysisResult createDiamond(efa::Block::OperationsVector &&Header,
                                        efa::Block::OperationsVector &&Left,
                                        efa::Block::OperationsVector &&Right,
                                        efa::Block::OperationsVector &&Footer) {
  efa::Function F;

  auto *HeaderBlock = F.addNode();
  HeaderBlock->Operations = Header;

  auto *LeftBlock = F.addNode();
  LeftBlock->Operations = Left;

  auto *RightBlock = F.addNode();
  RightBlock->Operations = Right;

  auto *FooterBlock = F.addNode();
  FooterBlock->Operations = Footer;

  HeaderBlock->addSuccessor(LeftBlock);
  HeaderBlock->addSuccessor(RightBlock);
  LeftBlock->addSuccessor(FooterBlock);
  RightBlock->addSuccessor(FooterBlock);

  return { std::move(F), HeaderBlock, FooterBlock };
}

static TestAnalysisResult createLoop(efa::Block::OperationsVector &&Header,
                                     efa::Block::OperationsVector &&LoopHeader,
                                     efa::Block::OperationsVector &&LoopBody,
                                     efa::Block::OperationsVector &&Footer) {
  efa::Function F;

  auto *HeaderBlock = F.addNode();
  HeaderBlock->Operations = Header;

  auto *LoopHeaderBlock = F.addNode();
  LoopHeaderBlock->Operations = LoopHeader;

  auto *LoopBodyBlock = F.addNode();
  LoopBodyBlock->Operations = LoopBody;

  auto *FooterBlock = F.addNode();
  FooterBlock->Operations = Footer;

  // digraph {
  //   Header -> LoopHeader -> LoopBody -> LoopHeader -> Footer;
  // }
  HeaderBlock->addSuccessor(LoopHeaderBlock);
  LoopHeaderBlock->addSuccessor(LoopBodyBlock);
  LoopHeaderBlock->addSuccessor(FooterBlock);
  LoopBodyBlock->addSuccessor(LoopHeaderBlock);

  return { std::move(F), HeaderBlock, FooterBlock };
};

inline void testLivenessAnalysis() {
  using namespace efa;

  auto RunAnalysis = [](BlockNode *Entry) {
    return MFP::getMaximalFixedPoint(LivenessAnalysis(),
                                     Entry,
                                     {},
                                     {},
                                     { Entry })[Entry]
      .OutValue;
  };

  auto RunOnSingleNode =
    [&RunAnalysis](efa::Block::OperationsVector &&Operations) -> BitVector {
    return RunAnalysis(createSingleNode(std::move(Operations)).getEntryNode());
  };

  BitVector Result;

  // Only read a register
  Result = RunOnSingleNode({
    Operation(OperationType::Read, 0),
  });
  revng_check(Result.size() == 1);
  revng_check(Result[0]);

  // Read then write
  Result = RunOnSingleNode({
    Operation(OperationType::Read, 0),
    Operation(OperationType::Write, 0),
  });
  revng_check(Result.size() == 1);
  revng_check(Result[0]);

  // Write then read
  Result = RunOnSingleNode({
    Operation(OperationType::Write, 0),
    Operation(OperationType::Read, 0),
  });
  revng_check(Result.size() == 1);
  revng_check(not Result[0]);

  // Write another register before reading the target register
  Result = RunOnSingleNode({
    Operation(OperationType::Write, 1),
    Operation(OperationType::Read, 0),
    Operation(OperationType::Write, 0),
  });
  revng_check(Result.size() == 2);
  revng_check(Result[0]);
  revng_check(not Result[1]);

  auto RunOnDiamond =
    [&RunAnalysis](efa::Block::OperationsVector &&Header,
                   efa::Block::OperationsVector &&Left,
                   efa::Block::OperationsVector &&Right,
                   efa::Block::OperationsVector &&Footer) -> BitVector {
    return RunAnalysis(createDiamond(std::move(Header),
                                     std::move(Left),
                                     std::move(Right),
                                     std::move(Footer))
                         .Exit);
  };

  // Read in footer
  Result = RunOnDiamond({}, {}, {}, { Operation(OperationType::Read, 0) });
  revng_assert(Result.size() == 1 and Result[0]);

  // Read in header
  Result = RunOnDiamond({ Operation(OperationType::Read, 0) }, {}, {}, {});
  revng_assert(Result.size() == 1 and Result[0]);

  // Read in left
  Result = RunOnDiamond({}, { Operation(OperationType::Read, 0) }, {}, {});
  revng_assert(Result.size() == 1 and Result[0]);

  // Read on one path, write on the other
  Result = RunOnDiamond({},
                        { Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Read, 0) },
                        {});
  revng_assert(Result.size() == 1 and Result[0]);

  // Read in footer, write on both paths
  Result = RunOnDiamond({},
                        { Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Read, 0) });
  revng_assert(Result.size() == 1 and not Result[0]);

  // Read in footer, write on one path
  Result = RunOnDiamond({},
                        {},
                        { Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Read, 0) });
  revng_assert(Result.size() == 1 and Result[0]);

  auto RunOnLoop =
    [&RunAnalysis](efa::Block::OperationsVector &&Header,
                   efa::Block::OperationsVector &&LoopHeader,
                   efa::Block::OperationsVector &&LoopBody,
                   efa::Block::OperationsVector &&Footer) -> BitVector {
    return RunAnalysis(createLoop(std::move(Header),
                                  std::move(LoopHeader),
                                  std::move(LoopBody),
                                  std::move(Footer))
                         .Exit);
  };

  // Read in loop header, clobber in loop body
  Result = RunOnLoop({},
                     { Operation(OperationType::Read, 0) },
                     { Operation(OperationType::Write, 0) },
                     {});
  revng_assert(Result.size() == 1 and Result[0]);

  // Viceversa
  Result = RunOnLoop({},
                     { Operation(OperationType::Write, 0) },
                     { Operation(OperationType::Read, 0) },
                     {});
  revng_assert(Result.size() == 1 and not Result[0]);

  // WIP: test using unreachable stuff
}

inline void testReachingDefinitions() {
  using namespace efa;

  // WIP: actually check analysis results
  auto RunAnalysis = [](efa::Function &&F) {
    return MFP::getMaximalFixedPoint(ReachingDefinitions(F),
                                     F.getEntryNode(),
                                     {},
                                     {},
                                     { F.getEntryNode() })[nullptr]
      .OutValue;
  };

  auto RunOnSingleNode =
    [&RunAnalysis](efa::Block::OperationsVector &&Operations) {
      return RunAnalysis(createSingleNode(std::move(Operations)));
    };

  RunOnSingleNode({ Operation(OperationType::Write, 0) });
  // Yes

  RunOnSingleNode({ Operation(OperationType::Write, 0),
                    Operation(OperationType::Read, 0) });
  // No

  RunOnSingleNode({ Operation(OperationType::Write, 0),
                    Operation(OperationType::Read, 0),
                    Operation(OperationType::Write, 0) });
  // Yes

  auto RunOnDiamond = [&RunAnalysis](efa::Block::OperationsVector &&Header,
                                     efa::Block::OperationsVector &&Left,
                                     efa::Block::OperationsVector &&Right,
                                     efa::Block::OperationsVector &&Footer) {
    return RunAnalysis(std::move(createDiamond(std::move(Header),
                                               std::move(Left),
                                               std::move(Right),
                                               std::move(Footer))
                                   .Function));
  };

  // Write read on all paths
  RunOnDiamond({ Operation(OperationType::Write, 0) },
               { Operation(OperationType::Read, 0) },
               { Operation(OperationType::Read, 0) },
               {});
  // No

  // Write read on one paths
  RunOnDiamond({ Operation(OperationType::Write, 0) },
               { Operation(OperationType::Read, 0) },
               {},
               {});
  // No

  // Distinct writes on all paths
  RunOnDiamond({},
               { Operation(OperationType::Write, 0) },
               { Operation(OperationType::Write, 0) },
               {});
  // Yes

  // Write on only one path
  RunOnDiamond({}, {}, { Operation(OperationType::Write, 0) }, {});
  // Yes

  // WIP: test using unreachable stuff
}

#endif
