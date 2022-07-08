/// \file HTML.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ADT/Concepts.h"
#include "revng/EarlyFunctionAnalysis/ControlFlowGraph.h"
#include "revng/Model/Binary.h"
#include "revng/Yield/ControlFlow/FallthroughDetection.h"
#include "revng/Yield/Function.h"
#include "revng/Yield/HTML.h"
#include "revng/Yield/Support/Tag.h"

using yield::htmlTag::Tag;

namespace tags {

static constexpr auto Div = "div";
static constexpr auto Span = "span";

} // namespace tags

namespace attributes {

static constexpr auto scope = "data-scope";
static constexpr auto token = "data-token";
static constexpr auto locationDefinition = "data-location-definition";
static constexpr auto locationReferences = "data-location-references";
static constexpr auto modelEditPath = "data-model-edit-path";

} // namespace attributes

namespace tokenTypes {

static constexpr auto Label = "asm.label";
static constexpr auto LabelIndicator = "asm.label-indicator";
static constexpr auto CommentIndicator = "asm.comment-indicator";
static constexpr auto Mnemonic = "asm.mnemonic";
static constexpr auto MnemonicPrefix = "asm.mnemonic-prefix";
static constexpr auto MnemonicSuffix = "asm.mnemonic-suffix";
static constexpr auto ImmediateValue = "asm.immediate-value";
static constexpr auto MemoryOperand = "asm.memory-operand";
static constexpr auto Register = "asm.register";

} // namespace tokenTypes

namespace scopes {

static constexpr auto Indentation = "indentation";
static constexpr auto Function = "asm.function";
static constexpr auto BasicBlock = "asm.basic-block";
static constexpr auto Instruction = "asm.instruction";

} // namespace scopes

static std::string labelAddress(const MetaAddress &Address) {
  std::string Result = Address.toString();

  constexpr std::array ForbiddenCharacters = { ' ', ':', '!', '#',  '?',
                                               '<', '>', '/', '\\', '{',
                                               '}', '[', ']' };

  for (char &Character : Result)
    if (llvm::find(ForbiddenCharacters, Character) != ForbiddenCharacters.end())
      Character = '_';

  return Result;
}

static std::string label(const yield::BasicBlock &BasicBlock,
                         const yield::Function &Function,
                         const model::Binary &Binary) {
  std::string LabelName;
  std::string FunctionPath;
  if (auto Iterator = Binary.Functions.find(BasicBlock.Start);
      Iterator != Binary.Functions.end()) {
    LabelName = Iterator->name().str().str();
    FunctionPath = "/Functions/" + Iterator->Entry.toString() + "/CustomName";
  } else {
    LabelName = "basic_block_at_" + labelAddress(BasicBlock.Start);
  }
  using model::Architecture::getAssemblyLabelIndicator;
  auto LabelIndicator = getAssemblyLabelIndicator(Binary.Architecture);
  Tag LabelTag(tags::Span, LabelName);
  LabelTag.add(attributes::token, tokenTypes::Label);
  if (!FunctionPath.empty())
    LabelTag.add(attributes::modelEditPath, FunctionPath);

  return LabelTag.serialize()
         + Tag(tags::Span, LabelIndicator)
             .add(attributes::token, tokenTypes::LabelIndicator)
             .serialize();
}

static std::string indent() {
  return Tag(tags::Span, "  ")
    .add(attributes::scope, scopes::Indentation)
    .serialize();
}

static std::string targetPath(const MetaAddress &Target,
                              const yield::Function &Function,
                              const model::Binary &Binary) {
  if (auto Iterator = Binary.Functions.find(Target);
      Iterator != Binary.Functions.end()) {
    // The target is a function
    return "/function/" + Iterator->Entry.toString();
  } else if (auto Iterator = Function.ControlFlowGraph.find(Target);
             Iterator != Function.ControlFlowGraph.end()) {
    // The target is a basic block
    return "/basic-block/" + Function.Entry.toString() + "/"
           + Iterator->Start.toString();
  } else if (Target.isValid()) {
    for (auto BasicBlock : Function.ControlFlowGraph) {
      for (auto Instruction : BasicBlock.Instructions) {
        if (Instruction.Address == Target) {
          return "/instruction/" + Function.Entry.toString() + "/"
                 + BasicBlock.Start.toString() + "/" + Target.toString();
        }
      }
    }
  }
  return "";
}

static std::set<std::string> targets(const yield::BasicBlock &BasicBlock,
                                     const yield::Function &Function,
                                     const model::Binary &Binary) {

  static const efa::ParsedSuccessor UnknownTarget{
    .NextInstructionAddress = MetaAddress::invalid(),
    .OptionalCallAddress = MetaAddress::invalid()
  };

  std::set<std::string> Result;
  for (const auto &Edge : BasicBlock.Successors) {
    auto TargetPair = efa::parseSuccessor(*Edge, BasicBlock.End, Binary);
    if (TargetPair.NextInstructionAddress.isValid()) {
      std::string Path = targetPath(TargetPair.NextInstructionAddress,
                                    Function,
                                    Binary);
      if (!Path.empty()) {
        Result.insert(Path);
      }
    }
    if (TargetPair.OptionalCallAddress.isValid()) {
      std::string Path = targetPath(TargetPair.OptionalCallAddress,
                                    Function,
                                    Binary);
      if (!Path.empty()) {
        Result.insert(Path);
      }
    }
  }

  return Result;
}

static std::string tagTypeAsString(const yield::TagType::Values &Type) {
  switch (Type) {
  case yield::TagType::Immediate:
    return tokenTypes::ImmediateValue;
  case yield::TagType::Memory:
    return tokenTypes::MemoryOperand;
  case yield::TagType::Mnemonic:
    return tokenTypes::Mnemonic;
  case yield::TagType::MnemonicPrefix:
    return tokenTypes::MnemonicPrefix;
  case yield::TagType::MnemonicSuffix:
    return tokenTypes::MnemonicSuffix;
  case yield::TagType::Register:
    return tokenTypes::Register;
  case yield::TagType::Whitespace:
  case yield::TagType::Invalid:
    return "";
  default:
    revng_abort("Unknown tag type");
  }
}

static std::string
tokenTag(llvm::StringRef Buffer, const yield::TagType::Values &Tag) {
  std::string TagStr = tagTypeAsString(Tag);
  if (!TagStr.empty()) {
    return ::Tag(tags::Span, Buffer).add(attributes::token, TagStr).serialize();
  } else {
    return Buffer.str();
  }
}

static std::string taggedText(const yield::Instruction &Instruction) {
  revng_assert(!Instruction.Tags.empty(),
               "Tagless instructions are not supported");
  revng_assert(!Instruction.Disassembled.empty(),
               "Empty disassembled instructions are not supported");

  std::vector TagMap(Instruction.Disassembled.size(), yield::TagType::Invalid);
  for (yield::Tag Tag : Instruction.Tags) {
    revng_assert(Tag.Type != yield::TagType::Invalid,
                 "\"Invalid\" TagType encountered");
    for (size_t Index = Tag.From; Index < Tag.To; Index++) {
      TagMap[Index] = Tag.Type;
    }
  }

  std::string Result;
  std::string Buffer;
  yield::TagType::Values Tag = yield::TagType::Invalid;
  for (size_t Index = 0; Index < Instruction.Disassembled.size(); Index++) {
    if (Tag != TagMap[Index]) {
      Result += tokenTag(Buffer, Tag);
      Tag = TagMap[Index];
      Buffer.clear();
    }
    Buffer += Instruction.Disassembled[Index];
  }
  Result += tokenTag(Buffer, Tag);

  return Result;
}

static std::string instruction(const yield::Instruction &Instruction,
                               const yield::BasicBlock &BasicBlock,
                               const yield::Function &Function,
                               const model::Binary &Binary,
                               bool AddTargets = false) {

  // Tagged instruction body.
  std::string Result = taggedText(Instruction);
  size_t Tail = Instruction.Disassembled.size() + 1;

  Tag Out = Tag(tags::Div, std::move(Result))
              .add(attributes::scope, scopes::Instruction)
              .add(attributes::locationDefinition,
                   "/instruction/" + Function.Entry.toString() + "/"
                     + BasicBlock.Start.toString() + "/"
                     + Instruction.Address.toString());

  if (AddTargets) {
    auto Targets = targets(BasicBlock, Function, Binary);
    Out.add(attributes::locationReferences, Targets);
  }

  return Out.serialize();
}

static std::string basicBlock(const yield::BasicBlock &BasicBlock,
                              const yield::Function &Function,
                              const model::Binary &Binary,
                              std::string Label) {
  revng_assert(!BasicBlock.Instructions.empty());
  auto FromIterator = BasicBlock.Instructions.begin();
  auto ToIterator = std::prev(BasicBlock.Instructions.end());
  if (BasicBlock.HasDelaySlot) {
    revng_assert(BasicBlock.Instructions.size() > 1);
    --ToIterator;
  }

  std::string Result;
  for (auto Iterator = FromIterator; Iterator != ToIterator; ++Iterator) {
    Result += indent() + instruction(*Iterator, BasicBlock, Function, Binary)
              + "\n";
  }
  Result += indent()
            + instruction(*(ToIterator++), BasicBlock, Function, Binary, true)
            + "\n";

  return Tag(tags::Div, Label + (Label.empty() ? "" : "\n") + Result)
    .add(attributes::scope, scopes::BasicBlock)
    .add(attributes::locationDefinition,
         "/basic-block/" + Function.Entry.toString() + "/"
           + BasicBlock.Start.toString())
    .serialize();
}

template<bool ShouldMergeFallthroughTargets>
static std::string labeledBlock(const yield::BasicBlock &FirstBlock,
                                const yield::Function &Function,
                                const model::Binary &Binary) {
  std::string Result;
  std::string Label = label(FirstBlock, Function, Binary);

  if constexpr (ShouldMergeFallthroughTargets == false) {
    Result = basicBlock(FirstBlock, Function, Binary, std::move(Label)) + "\n";
  } else {
    auto BasicBlocks = yield::cfg::labeledBlock(FirstBlock, Function, Binary);
    if (BasicBlocks.empty())
      return "";

    bool IsFirst = true;
    for (const auto &BasicBlock : BasicBlocks) {
      Result += basicBlock(*BasicBlock, Function, Binary, IsFirst ? Label : "");
      IsFirst = false;
    }
    Result += "\n";
  }

  return Result;
}

std::string yield::html::functionAssembly(const yield::Function &Function,
                                          const model::Binary &Binary) {
  std::string Result;

  for (const auto &BasicBlock : Function.ControlFlowGraph) {
    Result += labeledBlock<true>(BasicBlock, Function, Binary);
  }

  return ::Tag(tags::Div, Result)
    .add(attributes::scope, scopes::Function)
    .add(attributes::locationDefinition,
         "/function/" + Function.Entry.toString())
    .serialize();
}

std::string yield::html::controlFlowNode(const MetaAddress &Address,
                                         const yield::Function &Function,
                                         const model::Binary &Binary) {
  auto Iterator = Function.ControlFlowGraph.find(Address);
  revng_assert(Iterator != Function.ControlFlowGraph.end());

  auto Result = labeledBlock<false>(*Iterator, Function, Binary);
  revng_assert(!Result.empty());

  return Result;
}
