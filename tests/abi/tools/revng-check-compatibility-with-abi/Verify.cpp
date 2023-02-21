/// \file Verify.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <algorithm>

#include "revng/ABI/Definition.h"
#include "revng/ABI/FunctionType/Layout.h"
#include "revng/Model/Binary.h"

#include "ABIArtifactParser.h"

void verifyABI(const TupleTree<model::Binary> &Binary,
               llvm::StringRef RuntimeArtifact,
               model::ABI::Values ABI);

struct VerificationHelper {
public:
  void verify(const abi::FunctionType::Layout &FunctionLayout,
              const abi::artifact::FunctionArtifact &Artifact) const;

public:
  const model::Architecture::Values Architecture;
  const abi::Definition &ABI;
  const bool IsLittleEndian;

public:
  llvm::StringRef FunctionName = "";
};

struct IterationAccessHelper {
public:
  IterationAccessHelper(const abi::artifact::Iteration &Iteration,
                        const model::Architecture::Values Architecture) :
    Iteration(Iteration),
    RegisterSize(model::Architecture::getPointerSize(Architecture)) {}

  std::optional<llvm::ArrayRef<std::byte>>
  registerValue(model::Register::Values Register) {
    auto Iterator = Iteration.Registers.find(Register);
    if (Iterator != Iteration.Registers.end())
      return Iterator->Bytes;
    else
      return std::nullopt;
  }

public:
  const abi::artifact::Iteration &Iteration;
  const std::size_t RegisterSize;
};

using VH = VerificationHelper;
void VH::verify(const abi::FunctionType::Layout &FunctionLayout,
                const abi::artifact::FunctionArtifact &Artifact) const {
  for (std::size_t Index = 0; Index < Artifact.Iterations.size(); ++Index) {
    IterationAccessHelper Helper(Artifact.Iterations[Index], Architecture);

    // Sort the argument registers
    auto ARegisters = FunctionLayout.argumentRegisters();
    auto OrderedRegisterList = ABI.sortArguments(std::move(ARegisters));

    // Compute the relevant stack slice.
    abi::FunctionType::Layout::Argument::StackSpan StackSpan{ 0, 0 };
    for (const auto &Argument : FunctionLayout.Arguments) {
      if (Argument.Stack.has_value()) {
        revng_assert(Argument.Stack->Size != 0);
        if (StackSpan.Size == 0) {
          StackSpan = *Argument.Stack;
        } else {
          if (Argument.Stack->Offset != StackSpan.Offset + StackSpan.Size)
            revng_abort("Only continuous stack arguments are supported.");

          StackSpan.Size += Argument.Stack->Size;
        }
      }
    }
    llvm::ArrayRef<std::byte> StackBytes = Helper.Iteration.Stack;
    StackBytes = StackBytes.slice(StackSpan.Offset, StackSpan.Size);
    revng_check(StackBytes.size() == StackSpan.Size,
                "The piece of stack provided by the artifact is "
                "insufficient to hold all the arguments.");

    // Check the arguments.
    std::size_t CurrentRegisterIndex = 0;
    for (const auto &Argument : Helper.Iteration.Arguments) {
      llvm::ArrayRef<std::byte> ArgumentBytes = Argument.Bytes;
      if (ArgumentBytes.empty()) {
        std::string Error = "The `Bytes` field of the artifact is empty for "
                            "the argument #"
                            + std::to_string(ArgumentIndex) + " of '"
                            + FunctionName.str() + "' function.";
        revng_abort(Error.c_str());
      }

      do {
        if (CurrentRegisterIndex < OrderedRegisterList.size()) {
          auto Current = OrderedRegisterList[CurrentRegisterIndex];
          auto RegValue = Helper.registerValue(Current);
          if (!RegValue.has_value()) {
            std::string Error = "Verification of '"
                                + model::Register::getName(Current).str()
                                + "' register failed: it's not specified by "
                                  "the artifact for '"
                                + FunctionName.str() + "'.";
            revng_abort(Error.c_str());
          }

          if (ArgumentBytes.take_front(RegValue->size()).equals(*RegValue)) {
            ++CurrentRegisterIndex;
            ArgumentBytes = ArgumentBytes.drop_front(RegValue->size());

            // Current register value checks out, go to the next one.
            continue;
          } else if (auto RegPiece = RegValue->take_front(ArgumentBytes.size());
                     RegPiece.equals(ArgumentBytes)) {
            ++CurrentRegisterIndex;
            ArgumentBytes = {};

            break; // The last part of the argument was found.
          }
        }

        if (!StackBytes.take_front(ArgumentBytes.size()).equals(ArgumentBytes))
          revng_abort("An argument cannot be found, it uses neither the "
                      "expected stack part nor the expected registers.");

        // Stack matches.
        auto Min = model::Architecture::getPointerSize(Architecture);
        StackBytes = StackBytes.drop_front(std::max(ArgumentBytes.size(), Min));
        ArgumentBytes = {};
      } while (!ArgumentBytes.empty());
    }

    if (CurrentRegisterIndex != OrderedRegisterList.size()) {
      std::string Error = "'" + FunctionName.str()
                          + "' signature indicates the need for more registers "
                            "to pass an argument than the maximum allowed "
                            "count.";
      revng_abort(Error.c_str());
    }

    if (!StackBytes.empty()) {
      std::string Error = "The combined stack argument size of '"
                          + FunctionName.str()
                          + "' is different from what was "
                            "expected.";
      revng_abort(Error.c_str());
    }

    // Check the return value.
    auto RVR = FunctionLayout.returnValueRegisters();
    auto ReturnValueRegisterList = ABI.sortReturnValues(std::move(RVR));
    if (!Helper.Iteration.ReturnValue.Bytes.empty()) {
      if (ReturnValueRegisterList.size() == 0) {
        std::string Error = "Verification of the return value of '"
                            + FunctionName.str()
                            + "' failed: found a return value that should not "
                              "be there";
        revng_abort(Error.c_str());
      }

      std::size_t UsedRegisterCounter = 0;
      llvm::ArrayRef ReturnValueBytes = Helper.Iteration.ReturnValue.Bytes;
      for (auto &CurrentRegister : ReturnValueRegisterList) {
        auto RegValue = Helper.registerValue(CurrentRegister);
        if (!RegValue.has_value()) {
          std::string Error = "Verification of '"
                              + model::Register::getName(CurrentRegister).str()
                              + "' register failed: it's not specified by the "
                                "artifact.";
          revng_abort(Error.c_str());
        }

        if (ReturnValueBytes.take_front(RegValue->size()).equals(*RegValue)) {
          ReturnValueBytes = ReturnValueBytes.drop_front(RegValue->size());
          ++UsedRegisterCounter;
        } else if (auto Piece = RegValue->take_front(ReturnValueBytes.size());
                   Piece.equals(ReturnValueBytes)) {
          ReturnValueBytes = {};
          ++UsedRegisterCounter;
        } else {
          std::string Error = "Verification of the return value of '"
                              + FunctionName.str()
                              + "' failed: unable to locate it.";
          revng_abort(Error.c_str());
        }
      }

      if (UsedRegisterCounter != ReturnValueRegisterList.size()) {
        std::string Error = "'" + FunctionName.str()
                            + "' signature indicates the need for more "
                              "registers "
                              "to return a value than the maximum allowed "
                              "count.";
        revng_abort(Error.c_str());
      }
    } else if (ReturnValueRegisterList.size() != 0) {
      std::string Error = "'" + FunctionName.str()
                          + "' signature does not mention a return value even "
                            "though it's expected.";
      revng_abort(Error.c_str());
    }
  }
}

void verifyABI(const TupleTree<model::Binary> &Binary,
               llvm::StringRef RuntimeArtifact,
               model::ABI::Values ABI) {
  model::Architecture::Values Architecture = model::ABI::getArchitecture(ABI);
  auto ParsedArtifact = abi::artifact::parse(RuntimeArtifact, Architecture);

  llvm::StringRef ArchitectureName = model::Architecture::getName(Architecture);
  revng_assert(ArchitectureName == ParsedArtifact.Architecture);

  const abi::Definition &Def = abi::Definition::get(ABI);
  VerificationHelper Helper{ Architecture, Def, ParsedArtifact.IsLittleEndian };

  std::size_t TestedCount = 0;
  for (auto &Function : Binary->Functions()) {
    const std::string &Name = Function.OriginalName();
    auto CurrentFunction = ParsedArtifact.Functions.find(Name);
    if (CurrentFunction == ParsedArtifact.Functions.end()) {
      // Ignore types not present in the artifact.
      continue;
    }

    ++TestedCount;
    Helper.FunctionName = Name;

    using Layout = abi::FunctionType::Layout;
    const model::Type *Prototype = Function.Prototype().getConst();
    revng_assert(Prototype != nullptr);
    if (auto *CABI = llvm::dyn_cast<model::CABIFunctionType>(Prototype)) {
      // Copy the prototype since we might have to modify it before testing.
      model::CABIFunctionType PrototypeCopy = *CABI;

      if (ABI != PrototypeCopy.ABI()) {
        // Workaround for dwarf sometimes misdetecting a very specific ABI,
        // for example, it labels `SystemV_x86_regparam_N` as just `SystemV_x86`
        // despite them being incompatible.
        std::array AllowedABIs = { model::ABI::SystemV_x86_regparm_3,
                                   model::ABI::SystemV_x86_regparm_2,
                                   model::ABI::SystemV_x86_regparm_1 };
        revng_assert(llvm::is_contained(AllowedABIs, ABI));
        PrototypeCopy.ABI() = ABI;
      }

      Helper.verify(Layout(PrototypeCopy), *CurrentFunction);
    } else if (auto *Raw = llvm::dyn_cast<model::RawFunctionType>(Prototype)) {
      Helper.verify(Layout(*Raw), *CurrentFunction);
    } else {
      // Ignore non-function types.
    }
  }

  revng_check(TestedCount == ParsedArtifact.Functions.size(),
              "Not every function from the artifact was found in the binary. "
              "Does the binary match the artifact?");
}
