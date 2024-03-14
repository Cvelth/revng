//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#pragma clang optimize off

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SetOperations.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/ABI/RegisterState.h"
#include "revng/ADT/ZipMapIterator.h"
#include "revng/EarlyFunctionAnalysis/ABIAnalysis.h"
#include "revng/EarlyFunctionAnalysis/BasicBlock.h"
#include "revng/EarlyFunctionAnalysis/Common.h"
#include "revng/Model/Binary.h"
#include "revng/Model/Generated/Early/Architecture.h"
#include "revng/Model/Generated/Early/Register.h"
#include "revng/Model/Register.h"
#include "revng/Support/Assert.h"
#include "revng/Support/BasicBlockID.h"
#include "revng/Support/Debug.h"
#include "revng/Support/FunctionTags.h"
#include "revng/Support/IRHelpers.h"
#include "revng/Support/MetaAddress.h"

#include "../EFA4.cpp"
#include "Analyses.h"

using namespace llvm;

// WIP: rename to Log
static Logger<> ABIAnalysesLog("abi-analyses");

namespace ABIAnalyses {
using RegisterState = abi::RegisterState::Values;

template void
ABIAnalyses::ABIAnalysesResults::dump<Logger<true>>(Logger<true> &,
                                                    const char *) const;

struct PartialAnalysisResults {
  // Per function analysis
  RegisterStateMap UAOF;
  RegisterStateMap DRAOF;

  // Per call site analysis
  std::map<std::pair<BasicBlockID, BasicBlock *>, RegisterStateMap> URVOFC;
  std::map<std::pair<BasicBlockID, BasicBlock *>, RegisterStateMap> RAOFC;
  std::map<std::pair<BasicBlockID, BasicBlock *>, RegisterStateMap> DRVOFC;

  // Per return analysis
  std::map<std::pair<BasicBlockID, BasicBlock *>, RegisterStateMap> URVOF;

  // Debug methods
  void dump() const debug_function { dump(dbg, ""); }

  template<typename T>
  void dump(T &Output, const char *Prefix) const;
};

// Print the analysis results
template<typename T>
void PartialAnalysisResults::dump(T &Output, const char *Prefix) const {
  Output << Prefix << "UsedArgumentsOfFunction:\n";
  for (auto &[GV, State] : UAOF) {
    Output << Prefix << "  " << GV->getName().str() << " = "
           << abi::RegisterState::getName(State).str() << '\n';
  }

  Output << Prefix << "DeadRegisterArgumentsOfFunction:\n";
  for (auto &[GV, State] : DRAOF) {
    Output << Prefix << "  " << GV->getName().str() << " = "
           << abi::RegisterState::getName(State).str() << '\n';
  }

  Output << Prefix << "UsedReturnValuesOfFunctionCall:\n";
  for (auto &[Key, StateMap] : URVOFC) {
    Output << Prefix << "  " << getName(Key.second) << '\n';
    for (auto &[GV, State] : StateMap) {
      Output << Prefix << "    " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
  }

  Output << Prefix << "RegisterArgumentsOfFunctionCall:\n";
  for (auto &[Key, StateMap] : RAOFC) {
    Output << Prefix << "  " << getName(Key.second) << '\n';
    for (auto &[GV, State] : StateMap) {
      Output << Prefix << "    " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
  }

  Output << Prefix << "DeadReturnValuesOfFunctionCall:\n";
  for (auto &[Key, StateMap] : DRVOFC) {
    Output << Prefix << "  " << getName(Key.second) << '\n';
    for (auto &[GV, State] : StateMap) {
      Output << Prefix << "    " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
  }

  Output << Prefix << "UsedReturnValuesOfFunction:\n";
  for (auto &[Key, StateMap] : URVOF) {
    Output << Prefix << "  " << Key.second->getName().str() << '\n';
    for (auto &[GV, State] : StateMap) {
      Output << Prefix << "    " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
  }
}

RegisterState combine(RegisterState LH, RegisterState RH) {
  switch (LH) {
  case RegisterState::Yes:
    switch (RH) {
    case RegisterState::Yes:
    case RegisterState::YesOrDead:
    case RegisterState::Maybe:
      return RegisterState::Yes;
    case RegisterState::No:
    case RegisterState::NoOrDead:
    case RegisterState::Dead:
    case RegisterState::Contradiction:
      return RegisterState::Contradiction;
    case RegisterState::Count:
    case RegisterState::Invalid:
      revng_abort();
    }
    break;

  case RegisterState::YesOrDead:
    switch (RH) {
    case RegisterState::Yes:
      return RegisterState::Yes;
    case RegisterState::Maybe:
    case RegisterState::YesOrDead:
      return RegisterState::YesOrDead;
    case RegisterState::Dead:
    case RegisterState::NoOrDead:
      return RegisterState::Dead;
    case RegisterState::No:
    case RegisterState::Contradiction:
      return RegisterState::Contradiction;
    case RegisterState::Count:
    case RegisterState::Invalid:
      revng_abort();
    }
    break;

  case RegisterState::No:
    switch (RH) {
    case RegisterState::No:
    case RegisterState::NoOrDead:
    case RegisterState::Maybe:
      return RegisterState::No;
    case RegisterState::Yes:
    case RegisterState::YesOrDead:
    case RegisterState::Dead:
    case RegisterState::Contradiction:
      return RegisterState::Contradiction;
    case RegisterState::Count:
    case RegisterState::Invalid:
      revng_abort();
    }
    break;

  case RegisterState::NoOrDead:
    switch (RH) {
    case RegisterState::No:
      return RegisterState::No;
    case RegisterState::Maybe:
    case RegisterState::NoOrDead:
      return RegisterState::NoOrDead;
    case RegisterState::Dead:
    case RegisterState::YesOrDead:
      return RegisterState::Dead;
    case RegisterState::Yes:
    case RegisterState::Contradiction:
      return RegisterState::Contradiction;
    case RegisterState::Count:
    case RegisterState::Invalid:
      revng_abort();
    }
    break;

  case RegisterState::Dead:
    switch (RH) {
    case RegisterState::Dead:
    case RegisterState::Maybe:
    case RegisterState::NoOrDead:
    case RegisterState::YesOrDead:
      return RegisterState::Dead;
    case RegisterState::No:
    case RegisterState::Yes:
    case RegisterState::Contradiction:
      return RegisterState::Contradiction;
    case RegisterState::Count:
    case RegisterState::Invalid:
      revng_abort();
    }
    break;

  case RegisterState::Maybe:
    return RH;

  case RegisterState::Contradiction:
    return RegisterState::Contradiction;

  case RegisterState::Count:
  case RegisterState::Invalid:
    revng_abort();
  }
}

void finalizeReturnValues(ABIAnalysesResults &ABIResults) {
  for (auto &[PC, RSMap] : ABIResults.ReturnValuesRegisters) {
    for (auto &[CSV, RS] : RSMap) {
      if (!ABIResults.FinalReturnValuesRegisters.contains(CSV))
        ABIResults.FinalReturnValuesRegisters[CSV] = RegisterState::Maybe;

      ABIResults.FinalReturnValuesRegisters
        [CSV] = combine(ABIResults.FinalReturnValuesRegisters[CSV], RS);
    }
  }
}

struct CallSite {
  using Node = efa::Function::Node;

  Node *Block = nullptr;
  MetaAddress Callee;

  void setCallee(MetaAddress Callee) {
    if (this->Callee.isValid()) {
      revng_assert(this->Callee == Callee);
    } else {
      this->Callee = Callee;
    }
  }
};

struct FunctionToAnalyze {
  using Node = efa::Function::Node;

  efa::Function Function;

  std::map<BasicBlockID, CallSite> CallSites;

  /// An artificial return node sinking all the return instructions
  Node *ReturnNode = nullptr;

  /// An artificial node guaranteed to be reachable from all nodes
  Node *SinkNode = nullptr;
};

static model::Register::Values
getRegister(Value *Pointer, model::Architecture::Values Architecture) {
  if (auto *CSV = dyn_cast<GlobalVariable>(Pointer)) {
    return model::Register::fromCSVName(CSV->getName(), Architecture);
  }
  return model::Register::Invalid;
}

static efa::Function::Node *findUnrechableNode(efa::Function::Node *Start) {
  df_iterator_default_set<efa::Function::Node *> Visited;
  for (auto &_ : llvm::inverse_depth_first_ext(Start, Visited))
    ;

  for (auto *Node : Start->getParent()->nodes()) {
    if (not Visited.contains(Node))
      return Node;
  }

  return nullptr;
}

static efa::OperationType::Values storeType(Value *V) {
  Function *Callee = nullptr;
  if (auto *Call = dyn_cast<CallInst>(V))
    Callee = Call->getCalledFunction();

  if (Callee != nullptr and FunctionTags::ClobbererFunction.isTagOf(Callee)) {
    return efa::OperationType::Clobber;
  } else {
    return efa::OperationType::Write;
  }
}

static FunctionToAnalyze
fromLLVMFunction(llvm::Function &F,
                 model::Architecture::Values Architecture,
                 Function *PreCallSiteHook,
                 Function *PostCallSiteHook,
                 Function *RetHook) {
  F.dump();
  using namespace model;

  auto StackPointer = Architecture::getStackPointer(Architecture);

  SmallPtrSet<efa::Function::Node *, 16> Preserve;

  FunctionToAnalyze Result;
  efa::Function &Function = Result.Function;
  DenseMap<llvm::BasicBlock *, efa::BlockNode *> BlocksMap;

  Result.ReturnNode = Function.addNode();
  Result.ReturnNode->Label = "ReturnNode";
  Preserve.insert(Result.ReturnNode);

  Result.SinkNode = Function.addNode();
  Result.SinkNode->Label = "SinkNode";
  Result.ReturnNode->addSuccessor(Result.SinkNode);
  Preserve.insert(Result.SinkNode);

  uint8_t RegistersCount = 0;

  // Translate the function
  // Note: we use depth_first to avoid unreachable code
  for (llvm::BasicBlock *BB : llvm::depth_first(&F)) {
    // Create the node for the basic block
    auto *NewNode = Function.addNode();
    NewNode->Label = BB->getName();
    BlocksMap[BB] = NewNode;

    auto &Operations = NewNode->Operations;
    auto CreateOperation = [&Function,
                            &Operations,
                            &Architecture,
                            StackPointer](efa::OperationType::Values Type,
                                          Value *Pointer) {
      auto Register = getRegister(Pointer, Architecture);
      if (Register != model::Register::Invalid and Register != StackPointer) {
        Operations.emplace_back(Type, Function.registerIndex(Register));
      }
    };

    // Translate the basic block
    for (llvm::Instruction &I : *BB) {
      auto Call = dyn_cast<CallInst>(&I);

      if (auto *Load = dyn_cast<LoadInst>(&I)) {
        // If reading a register, record it
        CreateOperation(efa::OperationType::Read, Load->getPointerOperand());
      } else if (auto *Store = dyn_cast<StoreInst>(&I)) {
        // If writing a register, record it
        CreateOperation(storeType(Store->getValueOperand()),
                        Store->getPointerOperand());
      } else if (Call != nullptr
                 and (isCallTo(Call, PreCallSiteHook)
                      or isCallTo(Call, PostCallSiteHook)
                      or isCallTo(Call, RetHook))) {
        // Handle markers

        // Get the pc
        BasicBlockID PC = BasicBlockID::fromValue(Call->getArgOperand(0));
        revng_assert(PC.isValid());

        if (isCallTo(Call, PreCallSiteHook)) {
          // We found a call to pre_hook. This a llvm::BasicBlock starting with
          // pre_hook, ending with post_hook and containing register clobbering
          auto &CallSite = Result.CallSites[PC];

          // Ensure it's the first instruction in the basic block
          revng_assert(&*BB->begin() == &I);
          revng_assert(isCallTo(&*BB->getTerminator()->getPrevNode(),
                                PostCallSiteHook));

          // Record the current node as before a call
          CallSite.Block = NewNode;
          Preserve.insert(NewNode);

          // Record the callee
          CallSite.setCallee(MetaAddress::fromValue(Call->getArgOperand(1)));

        } else if (isCallTo(Call, PostCallSiteHook)) {
          // Perform some safety checks
          revng_assert(&*BB->getTerminator()->getPrevNode() == &I);
          revng_assert(Result.CallSites.contains(PC));
          revng_assert(Result.CallSites.at(PC).Block == NewNode);
        } else if (isCallTo(Call, RetHook)) {
          // This is a proper return, link to the exit node
          NewNode->addSuccessor(Result.ReturnNode);
        }
      }
    }
  }

  // Set the entry node
  efa::Function::Node *Entry = BlocksMap[&F.getEntryBlock()];
  Function.setEntryNode(Entry);
  Preserve.insert(Entry);

  // Create edges
  for (auto &[LLVMBlock, Node] : BlocksMap) {
    for (llvm::BasicBlock *Successor : successors(LLVMBlock)) {
      revng_assert(BlocksMap.count(Successor) != 0);
      Node->addSuccessor(BlocksMap[Successor]);
    }
  }

  // Ensure every node reaches the sink
  using Node = efa::Function::Node;
  Node *MaybeUnrechableNode = findUnrechableNode(Result.SinkNode);
  while (MaybeUnrechableNode != nullptr) {
    Result.SinkNode->addPredecessor(MaybeUnrechableNode);
    MaybeUnrechableNode = findUnrechableNode(Result.SinkNode);
  }

  // Perform some semplifications on the IR
  // WIP: Result.Function.simplify(Preserve);

  return Result;
}

// Run the ABI analyses on the outlined function F. This function must have all
// the original function calls replaced with a basic block starting with a call
// to `precall_hook` followed by a summary of the side effects of the function
// followed by a call to `postcall_hook` and a basic block terminating
// instruction.
ABIAnalysesResults
analyzeOutlinedFunction(Function *F,
                        const GeneratedCodeBasicInfo &GCBI,
                        model::Architecture::Values Architecture,
                        Function *PreCallSiteHook,
                        Function *PostCallSiteHook,
                        Function *RetHook) {
  namespace UAOF = UsedArgumentsOfFunction;
  namespace DRAOF = DeadRegisterArgumentsOfFunction;
  namespace RAOFC = RegisterArgumentsOfFunctionCall;
  namespace URVOFC = UsedReturnValuesOfFunctionCall;
  namespace DRVOFC = DeadReturnValuesOfFunctionCall;
  namespace URVOF = UsedReturnValuesOfFunction;

  ABIAnalysesResults FinalResults;
  PartialAnalysisResults Results;

  // WIP: no need to recreate this each time (?)
  revng_log(ABIAnalysesLog, "Building graph for " << F->getName());
  auto Function = fromLLVMFunction(*F,
                                   Architecture,
                                   PreCallSiteHook,
                                   PostCallSiteHook,
                                   RetHook);

  if (ABIAnalysesLog.isEnabled()) {
    Function.Function.dump(ABIAnalysesLog);
    ABIAnalysesLog << DoLog;
  }

  auto GetRegisterName = model::Register::getRegisterName;

  auto *M = F->getParent();
  auto GetCSV = [&M](model::Register::Values Register) {
    auto *Result = M->getGlobalVariable(model::Register::getCSVName(Register),
                                        true);
    revng_assert(Result != nullptr);
    return Result;
  };

  {
    // Run the liveness analysis
    revng_log(ABIAnalysesLog, "Running Liveness");
    efa::LivenessAnalysis Liveness(Function.Function);
    auto AnalysisResult = MFP::getMaximalFixedPoint(Liveness,
                                                    &Function.Function,
                                                    Liveness.defaultValue(),
                                                    Liveness.defaultValue(),
                                                    { Function.ReturnNode });

    // Collect registers alive at the entry
    revng_log(ABIAnalysesLog, "Registers alive at the entry of the function:");
    efa::BlockNode *EntryNode = Function.Function.getEntryNode();
    const BitVector &EntryResult = AnalysisResult[EntryNode].OutValue;
    for (auto Register : Function.Function.registersInSet(EntryResult)) {
      // This register is alive at the entry of the function

      revng_log(ABIAnalysesLog, "  " << GetRegisterName(Register));

      // WIP:
      FinalResults
        .ArgumentsRegisters[GetCSV(Register)] = abi::RegisterState::Yes;
    }

    for (const auto &[PC, CallSite] : Function.CallSites) {
      auto &ResultsCallSite = FinalResults.CallSites[PC];
      revng_log(ABIAnalysesLog,
                "Registers alive after the call to "
                  << CallSite.Callee.toString() << " at " << PC.toString());
      auto *PostNode = CallSite.Block;
      const BitVector &CallSiteResult = AnalysisResult.at(PostNode).InValue;
      for (auto Register : Function.Function.registersInSet(CallSiteResult)) {
        // This register is alive after the call site

        // WIP:
        revng_log(ABIAnalysesLog, "  " << GetRegisterName(Register));
        ResultsCallSite.ReturnValuesRegisters[GetCSV(Register)] = abi::RegisterState::Yes;
      }
    }
  }

  {
    // Run the reaching definitions analysis
    revng_log(ABIAnalysesLog, "Running ReachingDefinitions");
    efa::ReachingDefinitions ReachingDefinitions(Function.Function);
    auto DefaultValue = ReachingDefinitions.defaultValue();
    auto *EntryNode = Function.Function.getEntryNode();
    auto AnalysisResult = MFP::getMaximalFixedPoint(ReachingDefinitions,
                                                    &Function.Function,
                                                    DefaultValue,
                                                    DefaultValue,
                                                    { EntryNode });

    auto Compute = [&AnalysisResult, &Function](efa::Function::Node *Node,
                                                bool Before) {
      const auto &SinkResults = AnalysisResult.at(Function.SinkNode).OutValue;

      const auto *NodeResults = &AnalysisResult.at(Node).OutValue;
      if (Before)
        NodeResults = &AnalysisResult.at(Node).InValue;

      return efa::ReachingDefinitions::compute(*NodeResults, SinkResults);
    };

    revng_log(ABIAnalysesLog,
              "Registers with at least one write that reaches the exit node of "
              "the function without ever being read:");
    efa::BlockNode *ExitNode = Function.ReturnNode;
    const BitVector &ExitResult = Compute(ExitNode, false);
    for (auto Register : Function.Function.registersInSet(ExitResult)) {
      // This register has at least one write that reaches the exit node of the
      // function without ever being read
      revng_log(ABIAnalysesLog, "  " << GetRegisterName(Register));

      // WIP:
      FinalResults
        .ReturnValuesRegisters[BasicBlockID::invalid()]
                              [GetCSV(Register)] = abi::RegisterState::Yes;
    }

    for (const auto &[PC, CallSite] : Function.CallSites) {
      auto &ResultsCallSite = FinalResults.CallSites[PC];
      revng_log(ABIAnalysesLog,
                "Registers with at least one write that reaches the call to "
                  << CallSite.Callee.toString() << " at " << PC.toString()
                  << " without ever being read:");

      auto *PreNode = CallSite.Block;
      const BitVector &CallSiteResult = Compute(PreNode, true);
      for (auto Register : Function.Function.registersInSet(CallSiteResult)) {
        // This register has at least one write that reaches the call site
        // without ever being read

        revng_log(ABIAnalysesLog, "  " << GetRegisterName(Register));

        // WIP:
        ResultsCallSite
          .ArgumentsRegisters[GetCSV(Register)] = abi::RegisterState::Yes;
      }
    }
  }

  return FinalResults;

  // Initial population of partial results
  // TODO: merge the following analyses in a single one
  Results.UAOF = UAOF::analyze(&F->getEntryBlock(), GCBI);
  Results.DRAOF = DRAOF::analyze(&F->getEntryBlock(), GCBI);
  for (auto &I : instructions(F)) {
    BasicBlock *BB = I.getParent();

    if (auto *Call = dyn_cast<CallInst>(&I)) {
      BasicBlockID PC;
      if (isCallTo(Call, PreCallSiteHook) || isCallTo(Call, PostCallSiteHook)
          || isCallTo(Call, RetHook)) {
        PC = BasicBlockID::fromValue(Call->getArgOperand(0));
        revng_assert(PC.isValid());
      }

      if (isCallTo(Call, PreCallSiteHook)) {
        // Ensure it's the first instruction in the basic block
        revng_assert(&*BB->begin() == &I);

        Results.RAOFC[{ PC, BB }] = RAOFC::analyze(BB, GCBI);

        // Register address of the callee
        auto &CallSite = FinalResults.CallSites[PC];
        CallSite.CalleeAddress = MetaAddress::fromValue(Call->getArgOperand(1));
      } else if (isCallTo(Call, PostCallSiteHook)) {
        // Ensure it's the last instruction in the basic block
        revng_assert(&*BB->getTerminator()->getPrevNode() == &I);

        // TODO: merge the following analyses in a single one
        Results.URVOFC[{ PC, BB }] = URVOFC::analyze(BB, GCBI);
        Results.DRVOFC[{ PC, BB }] = DRVOFC::analyze(BB, GCBI);
      } else if (isCallTo(Call, RetHook)) {
        Results.URVOF[{ PC, BB }] = URVOF::analyze(BB, GCBI);
      }
    }
  }

  if (ABIAnalysesLog.isEnabled()) {
    ABIAnalysesLog << "Dumping ABIAnalyses results for function "
                   << F->getName() << ":\n";
    Results.dump(ABIAnalysesLog, "  ");
    ABIAnalysesLog << DoLog;
  }

  // Finalize results. Combine UAOF and DRAOF.
  for (auto &[Left, Right] : zipmap_range(Results.UAOF, Results.DRAOF)) {
    auto *CSV = Left == nullptr ? Right->first : Left->first;
    RegisterState LV = Left == nullptr ? RegisterState::Maybe : Left->second;
    RegisterState RV = Right == nullptr ? RegisterState::Maybe : Right->second;
    FinalResults.ArgumentsRegisters[CSV] = combine(LV, RV);
  }

  // Add RAOFC.
  for (auto &[Key, RSMap] : Results.RAOFC) {
    BasicBlockID PC = Key.first;
    revng_assert(PC.isValid());
    for (auto &[CSV, RS] : RSMap)
      FinalResults.CallSites[PC].ArgumentsRegisters[CSV] = RS;
  }

  // Combine URVOFC and DRVOFC.
  for (auto &[Key, _] : Results.URVOFC) {
    auto PC = Key.first;
    for (auto &[Left, Right] :
         zipmap_range(Results.URVOFC[Key], Results.DRVOFC[Key])) {
      auto *CSV = Left == nullptr ? Right->first : Left->first;
      RegisterState LV = Left == nullptr ? RegisterState::Maybe : Left->second;
      RegisterState RV = Right == nullptr ? RegisterState::Maybe :
                                            Right->second;
      FinalResults.CallSites[PC].ReturnValuesRegisters[CSV] = combine(LV, RV);
    }
  }

  // Add URVOF.
  for (auto &[Key, RSMap] : Results.URVOF) {
    auto PC = Key.first;
    for (auto &[CSV, RS] : RSMap)
      FinalResults.ReturnValuesRegisters[PC][CSV] = RS;
  }

  return FinalResults;
}

template<typename T>
void ABIAnalysesResults::dump(T &Output, const char *Prefix) const {
  Output << Prefix << "Arguments:\n";
  for (auto &[GV, State] : ArgumentsRegisters) {
    Output << Prefix << "  " << GV->getName().str() << " = "
           << abi::RegisterState::getName(State).str() << '\n';
  }

  Output << Prefix << "Call site:\n";
  for (auto &[PC, StateMap] : CallSites) {
    Output << Prefix << "  Call in basic block " << PC.toString() << '\n';
    Output << Prefix << "  "
           << "  "
           << "Arguments:\n";
    for (auto &[GV, State] : StateMap.ArgumentsRegisters) {
      Output << Prefix << "      " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
    Output << Prefix << "  "
           << "  "
           << "Return values:\n";
    for (auto &[GV, State] : StateMap.ReturnValuesRegisters) {
      Output << Prefix << "      " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
  }

  Output << Prefix << "Return values:\n";
  for (auto &[PC, StateMap] : ReturnValuesRegisters) {
    for (auto &[GV, State] : StateMap) {
      Output << Prefix << "  " << GV->getName().str() << " = "
             << abi::RegisterState::getName(State).str() << '\n';
    }
  }

  Output << Prefix << "Final Return values:\n";
  for (auto &[GV, State] : FinalReturnValuesRegisters) {
    Output << Prefix << "  " << GV->getName().str() << " = "
           << abi::RegisterState::getName(State).str() << '\n';
  }
}

} // namespace ABIAnalyses
