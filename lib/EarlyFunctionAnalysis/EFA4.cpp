/// \file EFA4.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <memory>

#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/GraphWriter.h"
#pragma clang optimize off

#include <cstdint>
#include <iterator>
#include <utility>

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/DOTGraphTraits.h"

#include "revng/ADT/GenericGraph.h"
#include "revng/MFP/MFP.h"
#include "revng/Model/Generated/Early/Register.h"
#include "revng/Model/Register.h"
#include "revng/Support/Generator.h"

using namespace llvm;

// WIP: clobber!

template<>
struct DenseMapInfo<model::Register::Values> {
  static model::Register::Values getEmptyKey() {
    return model::Register::Count;
  }

  static model::Register::Values getTombstoneKey() {
    return static_cast<model::Register::Values>(model::Register::Count + 1);
  }

  static unsigned getHashValue(const model::Register::Values &S) { return S; }

  static bool isEqual(const model::Register::Values &LHS,
                      const model::Register::Values &RHS) {
    return LHS == RHS;
  }
};

namespace efa {

namespace OperationType {

enum Values : uint8_t {
  Invalid,
  Read,
  Write,
  Clobber
};

inline llvm::StringRef getName(Values V) {
  switch (V) {
  case Invalid:
    return "Invalid";
  case Read:
    return "Read";
  case Write:
    return "Write";
  case Clobber:
    return "Clobber";
  default:
    revng_abort();
    break;
  }
}

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
  /// \note Only for debugging purposes
  std::string Label;
  OperationsVector Operations;

public:
  Block() = default;

public:
  std::string label() const {
    if (Label.size() > 0)
      return Label;
    else
      return ("0x" + Twine::utohexstr(reinterpret_cast<intptr_t>(this))).str();
  }

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

// WIP: implement simplify
// WIP: assert reachable
class Function : public GenericGraph<BlockNode> {
private:
  DenseMap<uint8_t, model::Register::Values> IndexToRegister;
  DenseMap<model::Register::Values, uint8_t> RegisterToIndex;

public:
  Function() = default;

public:
  uint8_t registerIndex(model::Register::Values Register) {
    auto It = RegisterToIndex.find(Register);
    if (It != RegisterToIndex.end())
      return It->second;

    auto RegistersCount = RegisterToIndex.size();
    RegisterToIndex[Register] = RegistersCount;
    IndexToRegister[RegistersCount] = Register;

    revng_assert(RegisterToIndex.size() == IndexToRegister.size());
    revng_assert(RegisterToIndex.size() == 1 + RegistersCount);

    return RegistersCount;
  }

  model::Register::Values registerByIndex(uint8_t Index) const {
    auto It = IndexToRegister.find(Index);
    revng_assert(It != IndexToRegister.end());
    return It->second;
  }

  uint8_t registersCount() const { return IndexToRegister.size(); }

  cppcoro::generator<model::Register::Values>
  registersInSet(const BitVector &Set) {
    for (unsigned Index : Set.set_bits()) {
      co_yield registerByIndex(Index);
    }
  }

  std::string toString(const Operation &Operation) const {
    auto Register = registerByIndex(Operation.Target);
    return (OperationType::getName(Operation.Type).str() + "("
            + model::Register::getName(Register).str() + ")");
  }

public:
  void simplify(const SmallPtrSetImpl<efa::Function::Node *> &ToPreserve) {
    dumpGraph();

    llvm::erase_if(Nodes, [&ToPreserve](std::unique_ptr<Node> &Owning) -> bool {
      auto *N = Owning.get();

      // Check preconditions
      if (N->predecessorCount() != 1)
        return false;

      Node *Predecessor = *N->predecessors().begin();
      if (Predecessor->successorCount() != 1)
        return false;

      revng_assert(*Predecessor->successors().begin() == N);

      if (N == Predecessor)
        return false;

      // Do not simplify nodes in ToPreserve
      if (ToPreserve.contains(N) or ToPreserve.contains(Predecessor))
        return false;

      for (Node *Successor : N->successors())
        if (Successor == N or Successor == Predecessor)
          return false;

      // WIP
      dbg << "Merging " << N->label() << " into " << Predecessor->label()
          << "\n";

      // Drop incoming edge
      N->clearPredecessors();
      revng_assert(not N->hasPredecessors());
      revng_assert(not Predecessor->hasSuccessors());

      // Move over successors
      for (auto &Successor : to_vector(N->successors()))
        Predecessor->addSuccessor(Successor);

      // Drop outgoing edges
      N->clearSuccessors();

      // Move operations
      for (Operation &Operation : N->Operations)
        Predecessor->Operations.push_back(Operation);

      // Drop
      return true;
    });
  }

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

public:
  template<typename S>
  void dump(S &Stream) const {
    for (const Node *N :
         llvm::ReversePostOrderTraversal<const Function *>(this)) {
      Stream << N->label() << ":\n";
      for (const Operation &Operation : N->Operations) {
        Stream << "  " << toString(Operation) << "\n";
      }

      Stream << "  Successors:\n";
      for (const Node *Successor : N->successors()) {
        Stream << "    " << Successor->label() << "\n";
      }

      Stream << "\n";
    }
  }

  void dump() const debug_function { dump(dbg); }

  void dumpGraph() const debug_function;
};

class LivenessAnalysis {
private:
  using Set = BitVector;
  using RegisterSet = Set;

public:
  using LatticeElement = Set;
  using GraphType = Inverse<const efa::Function *>;
  using Label = const BlockNode *;

private:
  Set Default;

public:
  LivenessAnalysis(const efa::Function &F) {
    uint8_t Max = 0;
    for (const Block *Block : F.nodes()) {
      for (const Operation &Operation : Block->Operations) {
        Max = std::max(Max, Operation.Target);
      }
    }

    Default.resize(Max + 1);
  }

public:
  Set defaultValue() const { return Default; }

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
public:
  struct RegisterWriters {
    /// Set of writes that reach this point alive
    BitVector Reaching;
    /// Set of writes that have been read on any path leading to this point
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

  /// One entry per register
  class WritersSet : public SmallVector<RegisterWriters, 16> {
  public:
    static WritersSet empty() { return {}; }

  public:
    void dump() const debug_function {
      for (unsigned I = 0; I < size(); ++I) {
        dbg << I << ":\n";

        dbg << "  Reaching: ";
        if ((*this)[I].Reaching.size() != 0)
          for (const auto &Word : (*this)[I].Reaching.getData())
            dbg << " " << Word;
        dbg << "\n";

        dbg << "  Read: ";
        if ((*this)[I].Read.size() != 0)
          for (const auto &Word : (*this)[I].Read.getData())
            dbg << " " << Word;
        dbg << "\n";
      }
    }
  };

public:
  using LatticeElement = WritersSet;
  using GraphType = efa::Function *;
  using Label = BlockNode *;

private:
  DenseMap<const Operation *, uint8_t> WriteToIndex;
  WritersSet Default = LatticeElement::empty();

public:
  ReachingDefinitions(const efa::Function &F) {
    // Populate WriteToIndex
    SmallVector<int> RegisterWriteIndex(F.registersCount(), 0);
    for (const Block *Block : F.nodes()) {
      for (const Operation &Operation : Block->Operations) {
        if (Operation.Type == OperationType::Write) {
          WriteToIndex[&Operation] = RegisterWriteIndex[Operation.Target];
          RegisterWriteIndex[Operation.Target] += 1;
        }
      }
    }

    Default.resize(RegisterWriteIndex.size());
    for (unsigned I = 0; I < RegisterWriteIndex.size(); ++I) {
      Default[I].Reaching.resize(RegisterWriteIndex[I]);
      Default[I].Read.resize(RegisterWriteIndex[I]);
    }
  }

public:
  static BitVector compute(const WritersSet &ProgramPoint,
                           const WritersSet &Sink) {
    BitVector Result;
    revng_assert(Sink.size() == ProgramPoint.size());
    Result.resize(ProgramPoint.size());

    unsigned Index = 0;
    for (const auto &[AtPoint, AtSink] : zip(ProgramPoint, Sink)) {
      auto Unread = AtSink.Read;
      Unread.flip();
      Result[Index] = AtPoint.Reaching.anyCommon(Unread);
      ++Index;
    }

    return Result;
  }

public:
  WritersSet defaultValue() const { return Default; }

public:
  WritersSet combineValues(const WritersSet &LHS, const WritersSet &RHS) const {
    WritersSet Result = LHS;
    for (const auto &[ResultEntry, RHSEntry] : zip(Result, RHS)) {
      ResultEntry |= RHSEntry;
    }
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

        Writes.Reaching.reset();

        if (Operation.Type == OperationType::Write) {
          Writes.Reaching.set(WriteToIndex.find(&Operation)->second);
        }

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

struct TestAnalysisResult {
  TestAnalysisResult() = delete;
  TestAnalysisResult(efa::Function &&Function,
                     efa::BlockNode *Entry,
                     efa::BlockNode *Exit,
                     efa::BlockNode *Sink) :
    Function(std::move(Function)), Entry(Entry), Exit(Exit), Sink(Sink) {}
  efa::Function Function;
  efa::BlockNode *Entry = nullptr;
  efa::BlockNode *Exit = nullptr;
  efa::BlockNode *Sink = nullptr;
};

static TestAnalysisResult
createSingleNode(efa::Block::OperationsVector &&Operations) {
  efa::Function F;
  F.registerIndex(model::Register::rax_x86_64);
  F.registerIndex(model::Register::rdi_x86_64);
  auto *Entry = F.addNode();
  F.setEntryNode(Entry);
  Entry->Operations = Operations;
  return { std::move(F), Entry, Entry, Entry };
}

static TestAnalysisResult createDiamond(efa::Block::OperationsVector &&Header,
                                        efa::Block::OperationsVector &&Left,
                                        efa::Block::OperationsVector &&Right,
                                        efa::Block::OperationsVector &&Footer) {
  efa::Function F;
  F.registerIndex(model::Register::rax_x86_64);
  F.registerIndex(model::Register::rdi_x86_64);

  auto *HeaderBlock = F.addNode();
  F.setEntryNode(HeaderBlock);
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

  return { std::move(F), HeaderBlock, FooterBlock, FooterBlock };
}

static TestAnalysisResult createLoop(efa::Block::OperationsVector &&Header,
                                     efa::Block::OperationsVector &&LoopHeader,
                                     efa::Block::OperationsVector &&LoopBody,
                                     efa::Block::OperationsVector &&Footer) {
  efa::Function F;
  F.registerIndex(model::Register::rax_x86_64);
  F.registerIndex(model::Register::rdi_x86_64);

  auto *HeaderBlock = F.addNode();
  F.setEntryNode(HeaderBlock);
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

  return { std::move(F), HeaderBlock, FooterBlock, FooterBlock };
};

static TestAnalysisResult
createNoReturn(efa::Block::OperationsVector &&Header,
               efa::Block::OperationsVector &&NoReturn,
               efa::Block::OperationsVector &&Exit) {
  efa::Function F;
  F.registerIndex(model::Register::rax_x86_64);
  F.registerIndex(model::Register::rdi_x86_64);

  auto *HeaderBlock = F.addNode();
  F.setEntryNode(HeaderBlock);
  HeaderBlock->Operations = Header;

  auto *NoReturnBlock = F.addNode();
  NoReturnBlock->Operations = NoReturn;

  auto *ExitBlock = F.addNode();
  ExitBlock->Operations = Exit;

  HeaderBlock->addSuccessor(NoReturnBlock);
  HeaderBlock->addSuccessor(ExitBlock);

  auto *SinkBlock = F.addNode();
  NoReturnBlock->addSuccessor((SinkBlock));
  ExitBlock->addSuccessor((SinkBlock));

  return { std::move(F), HeaderBlock, ExitBlock, SinkBlock };
}

inline void testLivenessAnalysis() {
  using namespace efa;

  auto RunAnalysis = [](efa::Function &Function, BlockNode *Entry) {
    LivenessAnalysis LA(Function);
    return MFP::getMaximalFixedPoint(LA,
                                     &Function,
                                     LA.defaultValue(),
                                     LA.defaultValue(),
                                     { Entry });
  };

  auto RunOnSingleNode =
    [&RunAnalysis](efa::Block::OperationsVector &&Operations) -> BitVector {
    auto Graph = createSingleNode(std::move(Operations));
    return RunAnalysis(Graph.Function, Graph.Entry)[Graph.Entry].OutValue;
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
    auto Graph = createDiamond(std::move(Header),
                               std::move(Left),
                               std::move(Right),
                               std::move(Footer));
    return RunAnalysis(Graph.Function, Graph.Exit)[Graph.Entry].OutValue;
  };

  // Read in footer
  Result = RunOnDiamond({}, {}, {}, { Operation(OperationType::Read, 0) });
  revng_assert(Result.size() == 1 and Result[0]);

  // Read in header
  Result = RunOnDiamond({ Operation(OperationType::Read, 0) }, {}, {}, {});
  revng_assert(Result.size() == 1);
  revng_assert(Result[0]);

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
    auto Graph = createLoop(std::move(Header),
                            std::move(LoopHeader),
                            std::move(LoopBody),
                            std::move(Footer));
    return RunAnalysis(Graph.Function, Graph.Exit)[Graph.Entry].OutValue;
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

  auto RunOnNoReturn =
    [&RunAnalysis](efa::Block::OperationsVector &&Header,
                   efa::Block::OperationsVector &&NoReturn,
                   efa::Block::OperationsVector &&Exit) -> BitVector {
    auto Graph = createNoReturn(std::move(Header),
                                std::move(NoReturn),
                                std::move(Exit));
    return RunAnalysis(Graph.Function, Graph.Exit)[Graph.Entry].OutValue;
  };

  // Read in noreturn block
  Result = RunOnNoReturn({}, { Operation(OperationType::Read, 0) }, {});
  revng_check(Result.size() == 1);
  revng_check(Result[0]);

  // Read in noreturn block, but write in entry
  Result = RunOnNoReturn({ Operation(OperationType::Write, 0) },
                         { Operation(OperationType::Read, 0) },
                         {});
  revng_check(Result.size() == 1);
  revng_check(not Result[0]);
}

inline void testReachingDefinitions() {
  using namespace efa;

  auto RunAnalysis = [](TestAnalysisResult &&F) {
    ReachingDefinitions RD(F.Function);
    auto Results = MFP::getMaximalFixedPoint(RD,
                                             &F.Function,
                                             RD.defaultValue(),
                                             RD.defaultValue(),
                                             { F.Entry });
    return ReachingDefinitions::compute(Results[F.Exit].OutValue,
                                        Results[F.Sink].OutValue);
  };

  auto RunOnSingleNode =
    [&RunAnalysis](efa::Block::OperationsVector &&Operations) {
      return RunAnalysis(createSingleNode(std::move(Operations)));
    };

  auto Result = RunOnSingleNode({ Operation(OperationType::Write, 0) });
  revng_assert(Result[0]);

  Result = RunOnSingleNode({ Operation(OperationType::Write, 0),
                             Operation(OperationType::Read, 0) });
  revng_assert(not Result[0]);

  Result = RunOnSingleNode({ Operation(OperationType::Write, 0),
                             Operation(OperationType::Read, 0),
                             Operation(OperationType::Write, 0) });
  revng_assert(Result[0]);

  auto RunOnDiamond = [&RunAnalysis](efa::Block::OperationsVector &&Header,
                                     efa::Block::OperationsVector &&Left,
                                     efa::Block::OperationsVector &&Right,
                                     efa::Block::OperationsVector &&Footer) {
    return RunAnalysis(createDiamond(std::move(Header),
                                     std::move(Left),
                                     std::move(Right),
                                     std::move(Footer)));
  };

  // Write read on all paths
  Result = RunOnDiamond({ Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Read, 0) },
                        { Operation(OperationType::Read, 0) },
                        {});
  revng_assert(not Result[0]);

  // Write read on one paths
  Result = RunOnDiamond({ Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Read, 0) },
                        {},
                        {});
  revng_assert(not Result[0]);

  // Distinct writes on all paths
  Result = RunOnDiamond({},
                        { Operation(OperationType::Write, 0) },
                        { Operation(OperationType::Write, 0) },
                        {});
  revng_assert(Result[0]);

  // Write on only one path
  Result = RunOnDiamond({}, {}, { Operation(OperationType::Write, 0) }, {});
  revng_assert(Result[0]);

  auto RunOnLoop = [&RunAnalysis](efa::Block::OperationsVector &&Header,
                                  efa::Block::OperationsVector &&LoopHeader,
                                  efa::Block::OperationsVector &&LoopBody,
                                  efa::Block::OperationsVector &&Footer) {
    return RunAnalysis(createLoop(std::move(Header),
                                  std::move(LoopHeader),
                                  std::move(LoopBody),
                                  std::move(Footer)));
  };

  // We read in the loop header, the write in the loop is always read
  Result = RunOnLoop({},
                     { Operation(OperationType::Read, 0) },
                     { Operation(OperationType::Write, 0) },
                     {});
  revng_assert(not Result[0]);

  // We read in the loop body, the write in the loop is read on at least one
  // path
  Result = RunOnLoop({},
                     { Operation(OperationType::Write, 0) },
                     { Operation(OperationType::Read, 0) },
                     {});
  revng_assert(not Result[0]);

  // We read in the function entry, the write in the loop is never read
  Result = RunOnLoop({ Operation(OperationType::Read, 0) },
                     { Operation(OperationType::Write, 0) },
                     {},
                     {});
  revng_assert(Result[0]);

  auto RunOnNoReturn = [&RunAnalysis](efa::Block::OperationsVector &&Header,
                                      efa::Block::OperationsVector &&NoReturn,
                                      efa::Block::OperationsVector &&Exit) {
    return RunAnalysis(createNoReturn(std::move(Header),
                                      std::move(NoReturn),
                                      std::move(Exit)));
  };

  // Dead write in the function entry
  Result = RunOnNoReturn({ Operation(OperationType::Write, 0) }, {}, {});
  revng_assert(Result[0]);

  // Dead write in the function exit
  Result = RunOnNoReturn({}, {}, { Operation(OperationType::Write, 0) });
  revng_assert(Result[0]);

  // Dead write in the noreturn block
  Result = RunOnNoReturn({}, { Operation(OperationType::Write, 0) }, {});
  revng_assert(not Result[0]);

  // Dead write in the exit block and the noreturn block
  Result = RunOnNoReturn({},
                         { Operation(OperationType::Write, 0) },
                         { Operation(OperationType::Write, 0) });
  revng_assert(Result[0]);
}

template<>
struct llvm::DOTGraphTraits<const efa::Function *>
  : public llvm::DefaultDOTGraphTraits {
  using EdgeIterator = llvm::GraphTraits<efa::Function *>::ChildIteratorType;
  DOTGraphTraits(bool IsSimple = false) : DefaultDOTGraphTraits(IsSimple) {}

  static std::string getNodeLabel(const efa::Function::Node *Node,
                                  const efa::Function *Graph) {
    std::string Label;
    Label += Node->label();
    Label += ":\\l";
    for (const efa::Operation &Operation : Node->Operations) {
      Label += "  " + Graph->toString(Operation) + "\\l";
    }

    return Label;
  }
};

inline void efa::Function::dumpGraph() const {
  llvm::WriteGraph(this, "efa-function");
}

// WIP: move to unit tests
struct XXX {
  XXX() {
    testLivenessAnalysis();
    testReachingDefinitions();
  }
};

inline XXX Mss;
