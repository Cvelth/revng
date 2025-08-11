//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <filesystem>
#include <string_view>
#include <system_error>

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ModuleSlotTracker.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/Model/NameBuilder.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Support/Assert.h"
#include "revng/Support/FunctionTags.h"
#include "revng/Support/InitRevng.h"
#include "revng/Support/MetaAddress.h"

namespace cl = llvm::cl;

static cl::OptionCategory ThisToolCategory("Tool options", "");

static cl::opt<std::string> InputDirectory(cl::Positional,
                                           cl::cat(ThisToolCategory),
                                           cl::desc("<input-directory>"));

static cl::opt<std::string> OutputFile("output", // formatting
                                       cl::cat(ThisToolCategory),
                                       cl::desc("<output-file>"));

static cl::opt<std::string> LostAddresses("lost",
                                          cl::cat(ThisToolCategory),
                                          cl::desc("<lost-address-list>"));

static cl::opt<std::string> VariableLocatability("variables",
                                                 cl::cat(ThisToolCategory),
                                                 cl::desc("<variable-"
                                                          "locatability-"
                                                          "data>"));

static std::map<uint64_t, std::string>
readFileList(std::string_view Directory) {
  namespace fs = std::filesystem;
  std::map<uint64_t, std::string> Result;
  for (auto File : fs::directory_iterator(Directory)) {
    std::string CurrentPath = File.path().string();
    llvm::StringRef Current = CurrentPath;
    revng_check(Current.consume_front(Directory));

    Current = Current.ltrim('/');
    revng_check(Current.consume_front("("));

    uint64_t Index = -1u;
    bool Consumed = !Current.consumeInteger<uint64_t>(10, Index);
    revng_assert(Consumed);
    revng_assert(Index != -1u);

    Result[Index] = std::move(CurrentPath);
  }

  return Result;
}

static const llvm::Module &getModule(llvm::StringRef Path) {
  static std::unordered_map<std::string, std::unique_ptr<llvm::Module>> Cache;

  auto CacheIterator = Cache.find(Path.str());
  if (CacheIterator != Cache.end())
    return *CacheIterator->second;

  static llvm::LLVMContext Context;

  llvm::SMDiagnostic Error;
  auto [Iterator,
        Success] = Cache.try_emplace(Path.str(),
                                     llvm::parseIRFile(Path, Error, Context));
  revng_assert(Success);

  revng_assert(Iterator->second);
  revng_assert(not Iterator->second->empty());

  // NOLINTNEXTLINE
  revng_assert(not llvm::verifyModule(*Iterator->second));

  return *Iterator->second;
}

static std::optional<MetaAddress>
tryExtractAddress(const llvm::Instruction &I) {
  if (!I.getDebugLoc() || !I.getDebugLoc()->getScope())
    return std::nullopt;

  auto DebugLocation = I.getDebugLoc()->getScope()->getName().str();
  auto Parsed = pipeline::locationFromString(revng::ranks::Instruction,
                                             DebugLocation);
  if (not Parsed.has_value())
    return std::nullopt;

  MetaAddress Extracted = Parsed->at(revng::ranks::Instruction);
  revng_assert(Extracted.isValid());
  return Extracted;
}

static SortedVector<MetaAddress>
gatherUserAddressList(const llvm::Instruction &I) {
  SortedVector<MetaAddress> Result;
  for (const llvm::Value *UserValue : I.users()) {
    if (const auto *User = llvm::dyn_cast<llvm::Instruction>(UserValue)) {
      if (std::optional MaybeAddress = tryExtractAddress(*User)) {
        Result.emplace(std::move(MaybeAddress.value()));

      } else {
        // Found a user without debug information,
        // discard current variable.
        return {};
      }
    }
  }

  return Result;
}

static bool isVariableInstruction(const llvm::Instruction &Instruction) {
  // NOTE: an ugly work-around, but it should suffice for not.
  return llvm::isa<llvm::AllocaInst>(&Instruction)
         || isCallToTagged(&Instruction, FunctionTags::LocalVariable);
}

static std::string simple_indent(llvm::StringRef Input, size_t Size) {
  std::string Result = Input.str();

  size_t Position = 0;
  while ((Position = Result.find('\n', Position)) != std::string::npos) {
    Result.replace(Position, 1, "\n" + std::string(Size, ' '));
    Position += 1 + Size;
  }

  return Result;
}

struct AnalysisResult {
  struct Step {
    llvm::StringRef Path;
    std::set<MetaAddress> UniqueAddresses = {};
    std::set<std::string> Addressless = {};
    std::uint64_t InstructionCount = 0;
    std::uint64_t BasicBlockCount = 0;
    std::uint64_t FunctionCount = 0;
  };
  std::vector<Step> Steps = {};
  std::map<MetaAddress, llvm::StringRef> Addresses = {};

public:
  void next(llvm::StringRef Path, llvm::StringRef NextName) {
    // An ugly workaround to a broken model
    if (Path.ends_with("calc-arm-static-revng-qa.compiled-stripped-48a95796/"
                       "(38) (pass) type-shrinking.ll")
        || Path.ends_with("calc-arm-static-revng-qa.compiled-with-debug-info-"
                          "2ee83210/(38) (pass) type-shrinking.ll")) {
      Step &Current = Steps.emplace_back(Steps.back());
      Current.Path = Path.rsplit('/').second;
      return;
    }

    Step &Current = Steps.emplace_back(Path.rsplit('/').second);

    const llvm::Module &Module = getModule(Path);
    llvm::ModuleSlotTracker Tracker(&Module);
    for (const llvm::Function &Function : Module) {
      if (not Function.getName().starts_with("local_"))
        continue;

      for (const llvm::BasicBlock &BasicBlock : Function) {
        for (const llvm::Instruction &Instruction : BasicBlock) {
          if (auto Address = tryExtractAddress(Instruction)) {
            Current.UniqueAddresses.emplace(*Address);
            Addresses[*Address] = NextName;

          } else {
            std::string Serialized;
            llvm::raw_string_ostream Stream(Serialized);
            Instruction.print(Stream, true);
            Current.Addressless.emplace(std::move(Serialized));
          }

          ++Current.InstructionCount;
        }

        ++Current.BasicBlockCount;
      }

      ++Current.FunctionCount;
    }
  }

  void variables(llvm::StringRef Path) {
    if (VariableLocatability == "")
      return;

    model::CNameBuilder RootBuilder(model::Binary{});
    std::vector<std::string> Locatable;
    std::vector<std::string> NonLocatable;

    const llvm::Module &Module = getModule(Path);
    llvm::ModuleSlotTracker Tracker(&Module);
    for (const llvm::Function &Function : Module) {
      if (not Function.getName().starts_with("local_"))
        continue;

      auto Builder = RootBuilder.localVariables(model::Function{});

      for (const llvm::BasicBlock &BasicBlock : Function) {
        for (const llvm::Instruction &Instruction : BasicBlock) {
          if (isVariableInstruction(Instruction)) {
            auto UserLocationSet = gatherUserAddressList(Instruction);
            auto NameResult = Builder.name(UserLocationSet);

            std::string *Output = nullptr;
            if (NameResult.HasAddressAssociated)
              Output = &Locatable.emplace_back("A locatable ");
            else
              Output = &NonLocatable.emplace_back("A non-locatable ");

            *Output += "variable (" + std::to_string(NameResult.Index)
                       + ") at '" + addressesToString(UserLocationSet)
                       + "' received the name: '" + NameResult.Name + "'";

            if (not NameResult.Warning.empty())
              *Output += "\n  Warning: " + NameResult.Warning;
          }
        }
      }
    }

    std::error_code ErrorCode;
    llvm::raw_fd_ostream Output(VariableLocatability, ErrorCode);
    if (ErrorCode)
      revng_abort(ErrorCode.message().c_str());

    Output << "LocatableCount: " << Locatable.size() << '\n';
    Output << "TotalCount: " << Locatable.size() + NonLocatable.size() << '\n';
    Output << "Percentage: "
           << std::to_string(double(Locatable.size())
                             / (Locatable.size() + NonLocatable.size()) * 100)
           << "%\n";
    Output << "Locatable:\n";
    for (const std::string &LogEntry : Locatable)
      Output << "- " << LogEntry << '\n';
    Output << "NonLocatable:\n";
    for (const std::string &LogEntry : NonLocatable)
      Output << "- " << LogEntry << '\n';
  }

  void dump() {
    if (OutputFile == "")
      return;

    std::error_code ErrorCode;
    llvm::raw_fd_ostream Output(OutputFile, ErrorCode);
    if (ErrorCode)
      revng_abort(ErrorCode.message().c_str());

    std::uint64_t FirstStepAddressCount = Steps.front().UniqueAddresses.size();

    Output << "Steps:\n";
    std::uint64_t PreviousStepCount = -1u;
    for (const auto &Step : Steps) {
      Output << "- Path: " << Step.Path.str() << "\n";
      Output << "  FunctionCount: " << Step.FunctionCount << "\n";
      Output << "  BasicBlockCount: " << Step.BasicBlockCount << "\n";
      Output << "  InstructionCount: " << Step.InstructionCount << "\n";
      Output << "  AddressCount: " << Step.UniqueAddresses.size() << "\n";

      if (PreviousStepCount != -1u) {
        Output << "  AddressesLost: "
               << (PreviousStepCount - Step.UniqueAddresses.size()) << "\n";
      }
      PreviousStepCount = Step.UniqueAddresses.size();

      if (Step.InstructionCount != 0) {
        Output << "  AddresslessPercent: "
               << std::to_string(double(Step.Addressless.size())
                                 / Step.InstructionCount * 100)
               << "%\n";
      }

      if (Step.Addressless.empty())
        Output << "  Offenders: []\n";
      else {
        Output << "  Offenders:\n";
        for (std::string_view Instruction : Step.Addressless)
          Output << "  - |\n  " << simple_indent(Instruction, 2) << '\n';
      }

      Output << "  Variety: "
             << std::to_string(double(Step.UniqueAddresses.size())
                               / FirstStepAddressCount * 100)
             << "%\n";
    }

    if (LostAddresses != "") {
      std::error_code ErrorCode;
      llvm::raw_fd_ostream Lost(LostAddresses, ErrorCode);
      if (ErrorCode)
        revng_abort(ErrorCode.message().c_str());

      for (auto [Address, LastSeen] : Addresses) {
        if (LastSeen != Steps.back().Path)
          Lost << Address.toString() << ": " << LastSeen << '\n';
      }
    }
  }
};

static llvm::StringRef simplifyName(llvm::StringRef FullName) {
  llvm::StringRef Result = FullName.rsplit('/').second;
  revng_check(Result.consume_front("("));

  uint64_t Index = -1u;
  bool Consumed = !Result.consumeInteger<uint64_t>(10, Index);
  revng_assert(Consumed);
  revng_assert(Index != -1u);

  revng_check(Result.consume_front(") ("));
  revng_check(Result.consume_front("pipe") or Result.consume_front("pass"));
  revng_check(Result.consume_front(") "));
  revng_check(Result.consume_back(".ll"));

  return Result;
}

int main(int ArgC, char *ArgV[]) {
  ::revng::InitRevng X(ArgC, ArgV, "", { &ThisToolCategory });

  auto FileList = readFileList(InputDirectory);
  revng_assert(!FileList.empty());

  std::vector<std::pair<std::string, uint64_t>> VarietyLoss;

  AnalysisResult Result;
  for (auto It = FileList.begin(); It != FileList.end(); ++It) {
    llvm::StringRef NextName = "SURVIVED";

    auto NextIt = It;
    if (++NextIt != FileList.end())
      NextName = simplifyName(NextIt->second);

    Result.next(It->second, NextName);
  }

  Result.variables((--FileList.end())->second);

  Result.dump();
}
