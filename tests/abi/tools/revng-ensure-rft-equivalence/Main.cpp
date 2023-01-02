//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

/// \file Main.cpp
/// \brief This tool is a specialized `revng model diff` extension designed
/// to work around the shortcomings of the abi testing pipeline.
///
/// It takes two models and produces a diff (and a non-zero exit code) if
/// they are not equivalent.
///
/// The main differences from `diff` is the fact that it invokes
/// `model::purgeUnnamedAndUnreachableTypes` on both models and also
/// specifically handles the IDs of `RawFunctionType`'s stack arguments.

#include <string>
#include <unordered_set>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/Model/Pass/PurgeUnnamedAndUnreachableTypes.h"
#include "revng/Model/ToolHelpers.h"
#include "revng/Support/InitRevng.h"
#include "revng/Support/MetaAddress/YAMLTraits.h"
#include "revng/Support/YAMLTraits.h"
#include "revng/TupleTree/TupleTree.h"
#include "revng/TupleTree/TupleTreeDiff.h"

using namespace llvm;

static cl::OptionCategory ThisToolCategory("Tool options", "");

static cl::opt<std::string> LeftModelPath(cl::Positional,
                                          cl::cat(ThisToolCategory),
                                          cl::desc("<left input model>"),
                                          cl::init("-"),
                                          cl::value_desc("left model"));

static cl::opt<std::string> RightModelPath(cl::Positional,
                                           cl::cat(ThisToolCategory),
                                           cl::desc("<right input model>"),
                                           cl::value_desc("right model"));

static cl::opt<std::string> OutputFilename("o",
                                           cl::cat(ThisToolCategory),
                                           cl::init("-"),
                                           llvm::cl::desc("Override "
                                                          "output "
                                                          "filename"),
                                           llvm::cl::value_desc("filename"));

static std::optional<std::pair<model::TypePath, model::TypePath>>
ensureMatchingIDs(const model::Type::Key &Left,
                  const model::Type::Key &Right,
                  model::Binary &Model) {
  auto &[LeftKind, LeftID] = Left;
  auto &[RightKind, RightID] = Right;
  revng_assert(LeftKind == RightKind);

  if (LeftID != RightID) {
    model::TypePath PathToModify = Model.getTypePath(Right);
    model::Type &ModifiedType = *PathToModify.get();

    // Since the key is getting modified, `batch_insert` makes sure the types
    // are sorted again at the end of this block.
    [[maybe_unused]] auto _ = Model.Types().batch_insert();

    // Replace the ID
    ModifiedType.ID() = LeftID;

    // Return the "replacement pair", so that the caller is able to gather them
    // and invoke `TupleTree::replaceReferences` on all of them at once.
    model::TypePath ModifiedPath = Model.getTypePath(ModifiedType.key());
    return std::make_pair(PathToModify, ModifiedPath);
  } else {
    return std::nullopt;
  }
}

int main(int Argc, char *Argv[]) {
  revng::InitRevng X(Argc, Argv);

  cl::HideUnrelatedOptions({ &ThisToolCategory });
  cl::ParseCommandLineOptions(Argc, Argv);

  ExitOnError ExitOnError;

  auto LeftModel = ModelInModule::load(LeftModelPath);
  if (not LeftModel)
    ExitOnError(LeftModel.takeError());

  auto RightModel = ModelInModule::load(RightModelPath);
  if (not RightModel)
    ExitOnError(RightModel.takeError());

  std::error_code EC;
  llvm::ToolOutputFile OutputFile(OutputFilename,
                                  EC,
                                  sys::fs::OpenFlags::OF_Text);
  if (EC)
    ExitOnError(llvm::createStringError(EC, EC.message()));

  // Introduce a function pair container to allow grouping them based on their
  // `CustomName`.
  struct FunctionPair {
    model::RawFunctionType *Left = nullptr;
    model::RawFunctionType *Right = nullptr;
  };
  struct TransparentComparator {
    using is_transparent = std::true_type;

    bool operator()(llvm::StringRef LHS, llvm::StringRef RHS) const {
      return LHS < RHS;
    }
  };
  std::map<model::Identifier, FunctionPair, TransparentComparator> Functions;
  std::unordered_set<std::size_t> FunctionIDLookup;

  // Make sure the default prototype is valid.
  revng_assert(LeftModel->Model->DefaultPrototype().isValid());
  const auto &DefaultPrototype = *LeftModel->Model->DefaultPrototype().get();

  // Gather all the `RawFunctionType`s prototypes present in the first model.
  for (model::Function &LF : LeftModel->Model->Functions()) {
    if (LF.Prototype().get() == nullptr)
      continue; // Skip functions without prototypes.

    revng_assert(LF.name() != "",
                 "This test uses names to differenciate functions, as such "
                 "having unnamed functions in the model would break it, "
                 "hence it's not allowed.");

    auto *Left = llvm::dyn_cast<model::RawFunctionType>(LF.Prototype().get());
    revng_assert(Left != nullptr);
    FunctionIDLookup.emplace(Left->ID());

    if (Left->ID() == DefaultPrototype.ID())
      continue; // Skip the default prototype.

    auto [Iterator, Success] = Functions.try_emplace(LF.name());
    if (!Success) {
      std::string Error = "Two functions share the name in the left model: "
                          + LF.name().str().str();
      revng_abort(Error.c_str());
    }
    Iterator->second.Left = Left;
  }

  // Gather all the `RawFunctionType`s prototypes present in the second model.
  for (model::Function &RF : RightModel->Model->Functions()) {
    if (RF.Prototype().get() == nullptr)
      continue; // Skip functions without prototypes.

    revng_assert(RF.name() != "",
                 "This test uses names to differenciate functions, as such "
                 "having unnamed functions in the model would break it, "
                 "hence it's not allowed.");

    auto *Right = llvm::dyn_cast<model::RawFunctionType>(RF.Prototype().get());
    revng_assert(Right != nullptr);

    if (Right->ID() == DefaultPrototype.ID())
      continue; // Skip the default prototype.

    auto Iterator = Functions.find(RF.name());
    if (Iterator == Functions.end()) {
      std::string Error = "A function present in the right model is missing in "
                          "the left one: "
                          + RF.name().str().str();
      revng_abort(Error.c_str());
    }
    revng_assert(Iterator->second.Right == nullptr,
                 "Two functions share the name in the right model");
    Iterator->second.Right = Right;
  }

  // Ensure both the functions themselves and their stack argument structs have
  // the same IDs. This makes it possible to use simple diff for comparing two
  // models instead of doing that manually, since the ID is the only piece
  // of the types that is allowed to change.
  std::map<model::TypePath, model::TypePath> LeftReplacements;
  std::map<model::TypePath, model::TypePath> RightReplacements;
  for (auto [Name, Pair] : Functions) {
    auto [Left, Right] = Pair;

    revng_assert(Left != nullptr,
                 "This should never happen, something is VERY wrong.");
    if (Right == nullptr) {
      std::string Error = "A function present in the left model is missing in "
                          "the right one: "
                          + Name.str().str();
      revng_abort(Error.c_str());
    }

    model::Binary &Binary = *RightModel->Model;
    if (auto R = ensureMatchingIDs(Left->key(), Right->key(), Binary))
      RightReplacements.emplace(std::move(R.value()));

    // Try and access the argument struct.
    revng_check(Left->StackArgumentsType().Qualifiers().empty());
    revng_check(Right->StackArgumentsType().Qualifiers().empty());
    auto *LeftStack = Left->StackArgumentsType().UnqualifiedType().get();
    auto *RightStack = Right->StackArgumentsType().UnqualifiedType().get();

    // XOR the `bool`eans - make sure that either both functions have stack
    // argument or neither one does.
    revng_check(!LeftStack == !RightStack);

    // Ignore function pairs without stack arguments.
    if (LeftStack == nullptr)
      continue;

    if (auto R = ensureMatchingIDs(LeftStack->key(), RightStack->key(), Binary))
      RightReplacements.emplace(std::move(R.value()));
  }

  // Gather all the pure function pointers, as in the types without
  // a `model::Function` using them.
  std::vector<const model::RawFunctionType *> LeftFunctionPointerTypes;
  for (const auto &Left : LeftModel->Model->Types())
    if (auto *Raw = llvm::dyn_cast<model::RawFunctionType>(Left.get()))
      if (!FunctionIDLookup.contains(Raw->ID()))
        LeftFunctionPointerTypes.emplace_back(Raw);
  std::vector<const model::RawFunctionType *> RightFunctionPointerTypes;
  for (const auto &Right : RightModel->Model->Types())
    if (auto *Raw = llvm::dyn_cast<model::RawFunctionType>(Right.get()))
      if (!FunctionIDLookup.contains(Raw->ID()))
        RightFunctionPointerTypes.emplace_back(Raw);

  // Remove these function pointers (and replace them with `void *`s) to avoid
  // them cluttering the diff result, since we can only reliably test real
  // functions (only ones with ABIArtifact available at that).
  model::TypePath Void;
  Void = LeftModel->Model->getPrimitiveType(model::PrimitiveTypeKind::Void, 0);
  for (const model::RawFunctionType *Type : LeftFunctionPointerTypes) {
    model::TypePath Reference = LeftModel->Model->getTypePath(Type->key());
    LeftReplacements.emplace(Reference, Void);
    LeftModel->Model->Types().erase(Type->key());
  }
  Void = RightModel->Model->getPrimitiveType(model::PrimitiveTypeKind::Void, 0);
  for (const model::RawFunctionType *Type : RightFunctionPointerTypes) {
    model::TypePath Reference = RightModel->Model->getTypePath(Type->key());
    RightReplacements.emplace(Reference, Void);
    RightModel->Model->Types().erase(Type->key());
  }

  // Replace references to modified types.
  LeftModel->Model.replaceReferences(LeftReplacements);
  RightModel->Model.replaceReferences(RightReplacements);

  // Erase the default prototypes because they interfere with the test.
  // NOTE: the types themselves should have already been replaced as they
  // qualify as function pointers (function types not used by any function).
  LeftModel->Model->DefaultPrototype() = model::TypePath{};
  RightModel->Model->DefaultPrototype() = model::TypePath{};

  // Streamline both models and diff them
  model::purgeUnnamedAndUnreachableTypes(LeftModel->Model);
  model::purgeUnnamedAndUnreachableTypes(RightModel->Model);
  auto Diff = diff(*LeftModel->Model, *RightModel->Model);

  if (Diff.Changes.empty()) {
    return EXIT_SUCCESS;
  } else {
    Diff.dump(OutputFile.os());
    return EXIT_FAILURE;
  }
}
