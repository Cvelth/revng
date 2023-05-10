//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <compare>

#include "revng/EarlyFunctionAnalysis/FunctionMetadata.h"
#include "revng/EarlyFunctionAnalysis/FunctionMetadataCache.h"
#include "revng/Lift/LoadBinaryPass.h"
#include "revng/Model/Binary.h"
#include "revng/Pipeline/AllRegistries.h"
#include "revng/Pipeline/Pipe.h"
#include "revng/Pipeline/RegisterPipe.h"
#include "revng/Pipes/Kinds.h"
#include "revng/Pipes/ModelGlobal.h"
#include "revng/Yield/Assembly/DisassemblyHelper.h"
#include "revng/Yield/Function.h"
#include "revng/Yield/PTML.h"
#include "revng/Yield/Pipes/EvaluateAssembly.h"
#include "revng/Yield/Pipes/ProcessAssembly.h"
#include "revng/Yield/Pipes/YieldAssembly.h"

using ptml::PTMLBuilder;

namespace revng::pipes {

void ProcessAssembly::run(pipeline::Context &Context,
                          const BinaryFileContainer &SourceBinary,
                          const pipeline::LLVMContainer &TargetList,
                          FunctionAssemblyStringMap &Output) {
  if (not SourceBinary.exists())
    return;

  // Access the model
  const auto &Model = getModelFromContext(Context);

  // Access the binary
  revng_assert(SourceBinary.path().has_value());
  auto MaybeBinary = loadBinary(*Model, *SourceBinary.path());
  revng_assert(MaybeBinary);
  const RawBinaryView &BinaryView = MaybeBinary->first;

  // Access the llvm module
  const llvm::Module &Module = TargetList.getModule();

  // Define the helper object to store the disassembly pipeline.
  // This allows it to only be created once.
  DissassemblyHelper Helper;

  FunctionMetadataCache Cache;
  for (const auto &LLVMFunction : FunctionTags::Isolated.functions(&Module)) {
    const auto &Metadata = Cache.getFunctionMetadata(&LLVMFunction);
    auto ModelFunctionIterator = Model->Functions().find(Metadata.Entry());
    revng_assert(ModelFunctionIterator != Model->Functions().end());

    const auto &Func = *ModelFunctionIterator;
    auto Disassembled = Helper.disassemble(Func, Metadata, BinaryView, *Model);
    Output.insert_or_assign(Func.Entry(), serializeToString(Disassembled));
  }
}

void ProcessAssembly::print(const pipeline::Context &,
                            llvm::raw_ostream &OS,
                            llvm::ArrayRef<std::string> Files) const {
  OS << "[this is a pure pipe, no command exists for its invocation]\n";
}

void YieldAssembly::run(pipeline::Context &Context,
                        const FunctionAssemblyStringMap &Input,
                        FunctionAssemblyPTMLStringMap &Output) {
  // Access the model
  const auto &Model = getModelFromContext(Context);

  PTMLBuilder ThePTMLBuilder;
  for (auto [Address, S] : Input) {
    auto MaybeFunction = TupleTree<yield::Function>::deserialize(S);
    revng_assert(MaybeFunction && MaybeFunction->verify());
    revng_assert((*MaybeFunction)->Entry() == Address);

    Output.insert_or_assign((*MaybeFunction)->Entry(),
                            yield::ptml::functionAssembly(ThePTMLBuilder,
                                                          **MaybeFunction,
                                                          *Model));
  }
}

void YieldAssembly::print(const pipeline::Context &,
                          llvm::raw_ostream &OS,
                          llvm::ArrayRef<std::string> Files) const {
  OS << "[this is a pure pipe, no command exists for its invocation]\n";
}

struct BasicBlockView {
  const BasicBlockID &BasicBlock;
  const yield::Function &Function;
  MetaAddress From;
  MetaAddress To;

public:
  explicit BasicBlockView(const yield::BasicBlock &BasicBlock,
                          const yield::Function &Function) :
    BasicBlock(BasicBlock.ID()),
    Function(Function),
    From(BasicBlock.ID().start()),
    To(BasicBlock.End()) {

    revng_assert(BasicBlock.ID().start().isCode());
    revng_assert(BasicBlock.End().isCode());
  }

  bool operator==(const BasicBlockView &Another) const {
    return From == Another.From && To == Another.To;
  }
  auto operator<(const BasicBlockView &Another) const {
    if (From.address() != Another.From.address())
      return From.address() < Another.From.address();
    else if (From != Another.From)
      return From < Another.From;
    else if (To.address() != Another.To.address())
      return To.address() > Another.To.address(); // reverse order.
    else
      return To > Another.To; // reverse order.
  }
};

struct FunctionComparator {
  bool operator()(const TupleTree<yield::Function> &LHS,
                  const TupleTree<yield::Function> &RHS) const {
    return LHS->key() < RHS->key();
  }
};

template<ConstexprString Characters>
std::size_t findAnyOf(const std::string &String, std::size_t Position) {
  std::size_t Result = std::string::npos;
  constexprRepeat<Characters.size()>([&]<std::size_t Idx>() {
    Result = std::min(Result, String.find(Characters[Idx], Position));
  });
  return Result;
}

void EvaluateAssembly::run(pipeline::Context &Context,
                           const FunctionAssemblyStringMap &Input,
                           AssemblyEvaluationFileContainer &Output) {
  // Access the model
  const TupleTree<model::Binary> &Model = getModelFromContext(Context);

  std::set<BasicBlockView> DeduplicatedBlocks;

  // Fill the block map deduplicating them in the process.
  std::set<TupleTree<yield::Function>, FunctionComparator> FunctionCache;
  for (auto [Address, Serialized] : Input) {
    auto MaybeFunction = TupleTree<yield::Function>::deserialize(Serialized);
    revng_assert(MaybeFunction && MaybeFunction->verify());
    revng_assert((*MaybeFunction)->Entry() == Address);
    auto [It, Success] = FunctionCache.emplace(std::move(*MaybeFunction));
    revng_assert(Success);

    dbg << "Basic blocks of " << serializeToString(Address) << ":\n";
    for (const auto &BasicBlock : (*It)->ControlFlowGraph()) {
      dbg << "  - " << serializeToString(BasicBlock.ID()) << '\n';
      DeduplicatedBlocks.emplace(BasicBlock, **It);
    }
  }

  dbg << "Final list:";
  for (const auto &Element : DeduplicatedBlocks)
    dbg << "  - " << serializeToString(Element.BasicBlock) << '\n';

  // Cut overlapping blocks up.
  for (auto Iterator = DeduplicatedBlocks.begin();
       Iterator != std::prev(DeduplicatedBlocks.end());) {
    const BasicBlockView &Next = *std::next(Iterator);
    if (*Iterator != Next && Next.From >= Iterator->From
        && Next.To <= Iterator->To) {
      if (Iterator->From == Next.From) {
        Iterator = DeduplicatedBlocks.erase(Iterator);
      } else {
        auto CurrentNode = DeduplicatedBlocks.extract(Iterator);
        CurrentNode.value().To = Next.From;

        auto Result = DeduplicatedBlocks.insert(std::move(CurrentNode));
        revng_assert(Result.inserted);
        Iterator = Result.position;
      }
    } else {
      ++Iterator;
    }
  }

  // Emit the assembly.
  std::string Result;
  PTMLBuilder ThePTMLBuilder(true);
  for (const auto &BlockView : DeduplicatedBlocks) {
    auto S = yield::ptml::blockPieceAssembly(ThePTMLBuilder,
                                             BlockView.BasicBlock,
                                             BlockView.Function,
                                             BlockView.From,
                                             BlockView.To,
                                             *Model);
    revng_assert(S.ends_with("\n\n"));
    S.pop_back();

    auto &BB = BlockView.Function.ControlFlowGraph().at(BlockView.BasicBlock);
    for (std::size_t Position = S.find('\n'), Index = 0, PreviousPosition = 0;
         Position != std::string::npos;
         Position = S.find('\n', Position + 1)) {

      std::string Current = S.substr(PreviousPosition,
                                     Position - PreviousPosition);
      if (PreviousPosition != 0) {
        revng_assert(Index < BB.Instructions().size());
        auto Address = std::next(BB.Instructions().begin(), Index++)->Address();
        Current.resize(64, ' ');
        Current += "   // at " + llvm::utohexstr(Address.address(), true);
      }

      Result += std::move(Current);
      PreviousPosition = Position;
    }

    std::string Targets;
    for (auto &Successor : BB.Successors()) {
      if (Successor->Destination().isValid()) {
        Targets += llvm::utohexstr(Successor->Destination().start().address(),
                                   true)
                   + ", ";
      }
    }

    if (!Targets.empty()) {
      Targets.resize(Targets.size() - 2);
      Result += "   // targets: " + std::move(Targets);
    }

    Result += '\n';
  }
  Output.setContent(std::move(Result));
}

void EvaluateAssembly::print(const pipeline::Context &,
                             llvm::raw_ostream &OS,
                             llvm::ArrayRef<std::string> Files) const {
  OS << "[this is a pure pipe, no command exists for its invocation]\n";
}

} // end namespace revng::pipes

using namespace revng::pipes;
static RegisterFunctionStringMap<FunctionAssemblyStringMap> X1;
static RegisterFunctionStringMap<FunctionAssemblyPTMLStringMap> X2;

static pipeline::RegisterPipe<revng::pipes::ProcessAssembly> ProcessPipe;
static pipeline::RegisterPipe<revng::pipes::YieldAssembly> YieldPipe;

using namespace pipeline;
static RegisterDefaultConstructibleContainer<AssemblyEvaluationFileContainer> X;
static RegisterPipe<revng::pipes::EvaluateAssembly> EvaluatePipe;
