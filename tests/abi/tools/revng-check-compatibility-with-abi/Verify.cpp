/// \file Verify.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <algorithm>

#include "revng/ABI/Definition.h"
#include "revng/ABI/FunctionType/Layout.h"
#include "revng/Model/Binary.h"

#include "ABIRuntimeTestResultParser.h"

void verifyABI(const TupleTree<model::Binary> &Binary,
               llvm::StringRef RuntimeArtifact,
               model::ABI::Values ABI);

struct VerificationHelper {
  const model::Architecture::Values Architecture;
  const abi::Definition &ABI;
  const bool IsLittleEndian;

public:
  llvm::StringRef FunctionName = "";

public:
  using ATest = abi::runtime_test::Named<abi::runtime_test::ArgumentTest>;
  void arguments(const abi::FunctionType::Layout &, const ATest &) const;

  using RVTest = abi::runtime_test::Named<abi::runtime_test::ReturnValueTest>;
  void returnValue(const abi::FunctionType::Layout &, const RVTest &) const;
};

using VH = VerificationHelper;
void VH::arguments(const abi::FunctionType::Layout &FunctionLayout,
                   const ATest &Iterations) const {
  for (const abi::runtime_test::ArgumentTest &Test : *Iterations) {
    // Sort argument registers
    auto ArgumentRegisters = FunctionLayout.argumentRegisters();
    auto OrderedRegisterList = ABI.sortArguments(std::move(ArgumentRegisters));

    // Compute the relevant piece of the stack leaving padding behind.
    std::vector<std::byte> ExtractedStackBytes;
    std::size_t PreviousArgumentEndsAt = 0;
    std::size_t CombinedArgumentSize = 0;
    for (const auto &Argument : FunctionLayout.Arguments) {
      if (Argument.Stack.has_value()) {
        revng_assert(Argument.Stack->Size != 0);
        if (Argument.Stack->Offset < PreviousArgumentEndsAt) {
          std::string Error = "Layout of '" + FunctionName.str()
                              + "' is not valid: stack arguments must not "
                                "overlap.";
          revng_abort(Error.c_str());
        }

        auto PaddingSize = Argument.Stack->Offset - PreviousArgumentEndsAt;
        if (PaddingSize > ABI.getPointerSize()) {
          // TODO: this check can be improved quite a bit by taking
          // `ABI::Types()` into the account.
          std::string Error = "Layout of '" + FunctionName.str()
                              + "' is not valid: padding exceeds the register "
                                "size.\n";
          Error += "The current argument is expected at offset "
                   + std::to_string(Argument.Stack->Offset)
                   + " while the previous one ends at "
                   + std::to_string(PreviousArgumentEndsAt) + "\n";
          revng_abort(Error.c_str());
        }

        std::size_t CurrentSize = ExtractedStackBytes.size();
        ExtractedStackBytes.resize(CurrentSize + Argument.Stack->Size);

        llvm::ArrayRef<std::byte> Bytes = Test.StateBeforeTheCall.Stack;
        Bytes = Bytes.slice(Argument.Stack->Offset, Argument.Stack->Size);
        llvm::copy(Bytes, std::next(ExtractedStackBytes.begin(), CurrentSize));
        PreviousArgumentEndsAt = Argument.Stack->Offset + Argument.Stack->Size;
        CombinedArgumentSize += (Argument.Stack->Size);
      }
    }
    llvm::ArrayRef<std::byte> StackBytes = ExtractedStackBytes;
    if (StackBytes.size() != CombinedArgumentSize) {
      std::string Error = "Verification of '" + FunctionName.str()
                          + "' failed: the piece of stack provided by the "
                            "artifact has a different size from what the "
                            "layout expects.";
      revng_abort(Error.c_str());
    }

    std::size_t CurrentRegisterIndex = 0;
    if (FunctionLayout.returnsAggregateType()) {
      // Account for the shadow pointer to the return value.
      revng_assert(not FunctionLayout.Arguments.empty());
      auto &ShadowArgument = FunctionLayout.Arguments[0];
      using namespace abi::FunctionType::ArgumentKind;
      revng_assert(ShadowArgument.Kind == ShadowPointerToAggregateReturnValue);
      if (ShadowArgument.Registers.size() == 1) {
        // It's in a register, drop one if needed.
        model::Register::Values Register = *ShadowArgument.Registers.begin();
        revng_assert(Register == ABI.ReturnValueLocationRegister());
        if (!OrderedRegisterList.empty())
          if (*OrderedRegisterList.begin() == ABI.ReturnValueLocationRegister())
            ++CurrentRegisterIndex;

      } else if (ShadowArgument.Stack.has_value()) {
        // It's on the stack, drop enough bytes for a pointer from the front.
        revng_assert(ShadowArgument.Stack->Offset == 0);
        revng_assert(ShadowArgument.Stack->Size == ABI.getPointerSize());
        StackBytes = StackBytes.drop_front(ABI.getPointerSize());
      } else {
        std::string Error = "Verification of the return value of '"
                            + FunctionName.str()
                            + "' failed: layout is not valid, did it verify?";
        revng_abort(Error.c_str());
      }
    }

    // Check the arguments.
    std::uint64_t ArgumentIndex = 0;
    for (const abi::runtime_test::Argument &Argument : Test.Arguments) {
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
          auto RegValue = Test.StateBeforeTheCall[Current];
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

        if (StackBytes.take_front(ArgumentBytes.size()).equals(ArgumentBytes)) {
          std::size_t BytesToDrop = ABI.paddedSizeOnStack(ArgumentBytes.size());
          if (StackBytes.size() < BytesToDrop)
            StackBytes = {};
          else
            StackBytes = StackBytes.drop_front(BytesToDrop);
          break; // Stack checks out, go to the next argument.
        }

        revng_assert(!ABI.ScalarTypes().empty());
        auto &BiggestScalarType = *std::prev(ABI.ScalarTypes().end());
        if (BiggestScalarType.alignedAt() != ABI.getPointerSize()) {
          // If the ABI supports unusual alignment, try to account for it,
          // by dropping an conflicting part of the stack data.
          if (StackBytes.size() < ABI.getPointerSize())
            StackBytes = {};
          else
            StackBytes = StackBytes.drop_front(ABI.getPointerSize());
        }

        if (StackBytes.take_front(ArgumentBytes.size()).equals(ArgumentBytes)) {
          std::size_t BytesToDrop = ABI.paddedSizeOnStack(ArgumentBytes.size());
          if (StackBytes.size() < BytesToDrop)
            StackBytes = {};
          else
            StackBytes = StackBytes.drop_front(BytesToDrop);
          break; // Stack matches after accounting for alignment.
        }

        std::string Error = "The argument #" + std::to_string(ArgumentIndex)
                            + " of '" + FunctionName.str()
                            + "' cannot be found: it uses neither the expected "
                              "stack part nor the expected registers.";
        revng_abort(Error.c_str());
      } while (!ArgumentBytes.empty());

      ++ArgumentIndex;
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
  }
}
using VH = VerificationHelper;
void VH::returnValue(const abi::FunctionType::Layout &FunctionLayout,
                     const RVTest &Iterations) const {
  for (const abi::runtime_test::ReturnValueTest &Test : *Iterations) {
    if (Test.ExpectedReturnValue.Bytes != Test.ReturnValue.Bytes) {
      std::string Error = "Return value of '" + FunctionName.str()
                          + "' was lost: something went *very* wrong.";
      revng_abort(Error.c_str());
    }

    if (FunctionLayout.returnsAggregateType()) {
      // The return value location is passed in as a pointer.
    } else {
      // The value is returned normally.
    }
  }

  // std::optional<model::Register::Values> SingleRegisterRV;
  // for (const abi::runtime_test::ReturnValueTest &Test : *Iterations) {
  //   auto RVR = FunctionLayout.returnValueRegisters();
  //   auto ReturnValueRegisterList = ABI.sortReturnValues(std::move(RVR));
  //   if (!Test.->Bytes.empty()) {
  //     if (ReturnValueRegisterList.size() == 0) {
  //       std::string Error = "Verification of the return value of '"
  //                           + FunctionName.str()
  //                           + "' failed: found a return value that should not
  //                           "
  //                             "be there";
  //       revng_abort(Error.c_str());
  //     }
  //
  //     std::size_t UsedRegisterCounter = 0;
  //     llvm::ArrayRef ReturnValueBytes = Iteration->Bytes;
  //     for (auto &CurrentRegister : ReturnValueRegisterList) {
  //       auto RegValue = Iteration.registerValue(CurrentRegister);
  //       if (!RegValue.has_value()) {
  //         std::string Error = "Verification of '"
  //                             +
  //                             model::Register::getName(CurrentRegister).str()
  //                             + "' register failed: it's not specified by the
  //                             "
  //                               "artifact.";
  //         revng_abort(Error.c_str());
  //       }
  //
  //       if (ReturnValueBytes.take_front(RegValue->size()).equals(*RegValue))
  //       {
  //         ReturnValueBytes = ReturnValueBytes.drop_front(RegValue->size());
  //         ++UsedRegisterCounter;
  //       } else if (auto Piece =
  //       RegValue->take_front(ReturnValueBytes.size());
  //                  Piece.equals(ReturnValueBytes)) {
  //         ReturnValueBytes = {};
  //         ++UsedRegisterCounter;
  //       } else {
  //         std::string Error = "Verification of the return value of '"
  //                             + FunctionName.str()
  //                             + "' failed: unable to locate it.";
  //         revng_abort(Error.c_str());
  //       }
  //     }
  //
  //     if (UsedRegisterCounter != ReturnValueRegisterList.size()) {
  //       std::string Error = "'" + FunctionName.str()
  //                           + "' signature indicates the need for more "
  //                             "registers "
  //                             "to return a value than the maximum allowed "
  //                             "count.";
  //       revng_abort(Error.c_str());
  //     }
  //
  //     if (ReturnValueRegisterList.size() == 1)
  //       SingleRegisterRV = ReturnValueRegisterList[0];
  //   } else if (ReturnValueRegisterList.size() != 0) {
  //     std::string Error = "'" + FunctionName.str()
  //                         + "' signature does not mention a return value even
  //                         "
  //                           "though it's expected.";
  //     revng_abort(Error.c_str());
  //   }
  // }
}

static abi::FunctionType::Layout
getPrototypeLayout(const model::Function &Function, model::ABI::Values ABI) {
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

    return abi::FunctionType::Layout(PrototypeCopy);
  } else if (auto *Raw = llvm::dyn_cast<model::RawFunctionType>(Prototype)) {
    return abi::FunctionType::Layout(*Raw);
  } else {
    revng_abort("Layouts of non-function types are not supported.");
  }
}

void verifyABI(const TupleTree<model::Binary> &Binary,
               llvm::StringRef RuntimeArtifact,
               model::ABI::Values ABI) {
  model::Architecture::Values Architecture = model::ABI::getArchitecture(ABI);
  auto Parsed = abi::runtime_test::parse(RuntimeArtifact, Architecture);

  llvm::StringRef ArchitectureName = model::Architecture::getName(Architecture);
  revng_check(ArchitectureName == Parsed.Architecture);

  const abi::Definition &Def = abi::Definition::get(ABI);
  VerificationHelper Helper{ Architecture, Def, Parsed.IsLittleEndian };

  std::size_t ArgumentTestCount = 0, ReturnValueTestCount = 0;
  for (auto &Function : Binary->Functions()) {
    Helper.FunctionName = Function.OriginalName();
    if (auto Test = Parsed.ArgumentTests.find(Helper.FunctionName);
        Test != Parsed.ArgumentTests.end()) {
      Helper.arguments(getPrototypeLayout(Function, ABI), *Test);
      ++ArgumentTestCount;
    } else if (auto Test = Parsed.ReturnValueTests.find(Helper.FunctionName);
               Test != Parsed.ReturnValueTests.end()) {
      Helper.returnValue(getPrototypeLayout(Function, ABI), *Test);
      ++ReturnValueTestCount;
    } else {
      // Ignore types from the model, that are not mentioned in
      // the runtime test artifact.
      continue;
    }
  }

  if (ArgumentTestCount != Parsed.ArgumentTests.size()) {
    std::string Error = std::to_string(ArgumentTestCount)
                        + " functions from the binary were tested, but "
                          "artifact contains "
                        + std::to_string(Parsed.ArgumentTests.size())
                        + " argument test functions.\n"
                          "Does the binary match the artifact?";
    revng_abort(Error.c_str());
  }

  if (ReturnValueTestCount != Parsed.ReturnValueTests.size()) {
    std::string Error = std::to_string(ReturnValueTestCount)
                        + " functions from the binary were tested, but "
                          "artifact contains "
                        + std::to_string(Parsed.ReturnValueTests.size())
                        + " return value test functions.\n"
                          "Does the binary match the artifact?";
    revng_abort(Error.c_str());
  }
}
