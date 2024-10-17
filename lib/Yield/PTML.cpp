/// \file PTML.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <unordered_map>
#include <unordered_set>

#include "llvm/ADT/PostOrderIterator.h"

#include "revng/ADT/Concepts.h"
#include "revng/EarlyFunctionAnalysis/CFGHelpers.h"
#include "revng/Model/Binary.h"
#include "revng/Model/Helpers.h"
#include "revng/PTML/Constants.h"
#include "revng/PTML/Doxygen.h"
#include "revng/PTML/Tag.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Yield/ControlFlow/FallthroughDetection.h"
#include "revng/Yield/Function.h"
#include "revng/Yield/PTML.h"

using pipeline::locationString;
using ptml::Tag;
namespace attributes = ptml::attributes;
namespace ptmlScopes = ptml::scopes;
namespace tags = ptml::tags;
namespace ranks = revng::ranks;

namespace tokenTypes {

static constexpr auto LabelIndicator = "asm.label-indicator";
static constexpr auto RawBytes = "asm.raw-bytes";
static constexpr auto InstructionAddress = "asm.instruction-address";

} // namespace tokenTypes

namespace scopes {

static constexpr auto Function = "asm.function";
static constexpr auto BasicBlock = "asm.basic-block";
static constexpr auto Instruction = "asm.instruction";

} // namespace scopes

static std::string targetPath(const BasicBlockID &Target,
                              const yield::Function &Function,
                              const model::Binary &Binary) {
  if (const auto *F = yield::tryGetFunction(Binary, Target)) {
    // The target is a function
    return locationString(ranks::Function, F->Entry());
  } else if (auto Iterator = Function.Blocks().find(Target);
             Iterator != Function.Blocks().end()) {
    // The target is a basic block
    return locationString(ranks::BasicBlock, Function.Entry(), Iterator->ID());
  } else if (Target.isValid()) {
    for (const auto &Block : Function.Blocks()) {
      if (Block.Instructions().contains(Target.start())) {
        // The target is an instruction
        return locationString(ranks::Instruction,
                              Function.Entry(),
                              Block.ID(),
                              Target.start());
      }
    }
  }

  revng_abort(("Unknown target:\n" + toString(Target)).c_str());
}

static std::set<std::string> targets(const yield::BasicBlock &BasicBlock,
                                     const yield::Function &Function,
                                     const model::Binary &Binary) {
  std::set<std::string> Result;
  for (const auto &Edge : BasicBlock.Successors()) {
    auto [NextAddress, MaybeCall] = efa::parseSuccessor(*Edge,
                                                        BasicBlock.nextBlock(),
                                                        Binary);
    if (NextAddress.isValid())
      Result.emplace(targetPath(NextAddress, Function, Binary));

    if (MaybeCall.isValid())
      Result.emplace(targetPath(BasicBlockID(MaybeCall), Function, Binary));
  }

  // Explicitly remove the next target if there is only a single other target,
  // i.e. it's a conditional jump, call, etc.
  if (Result.size() == 2) {
    auto NextBlock = targetPath(BasicBlock.nextBlock(), Function, Binary);
    if (auto Iterator = llvm::find(Result, NextBlock); Iterator != Result.end())
      Result.erase(Iterator);
  }

  return Result;
}

static std::string emitTagged(const ptml::MarkupBuilder &B,
                              const yield::TaggedString &String) {
  llvm::StringRef Type = yield::TagType::toPTML(String.Type());
  if (Type.empty()) {
    revng_assert(String.Attributes().empty());
    return std::move(String.Content());
  }

  auto Result = B.getTag(tags::Span, std::move(String.Content()))
                  .addAttribute(attributes::Token, std::move(Type));
  for (const yield::TagAttribute &Attribute : String.Attributes())
    Result.addAttribute(Attribute.Name(), Attribute.Value());

  return Result.toString();
}

static std::string taggedLine(const ptml::MarkupBuilder &B,
                              const SortedVector<yield::TaggedString> &Tagged) {
  std::string Result;

  for (const yield::TaggedString &String : Tagged)
    Result += emitTagged(B, String);

  return Result += '\n';
}

/// An internal helper for managing instruction prefixes.
///
/// It builds a map of instructions to prefixes for a passed function, and
/// then allows extracting them one by one using `emit` method, while making
/// sure all the calls to `emit` across the function returns strings of the same
/// length.
class InstructionPrefixManager {
private:
  struct InstructionPrefix {
    std::string Address;
    std::string Bytes;
  };

  std::map<BasicBlockID, std::unordered_map<MetaAddress, InstructionPrefix>>
    Prefixes;
  uint64_t LongestAddressString = 0;
  uint64_t LongestByteString = 0;

public:
  InstructionPrefixManager() {}
  InstructionPrefixManager(const yield::Function &Function,
                           const model::Binary &Binary) {
    const auto Config = Binary.Configuration().Disassembly();
    for (const yield::BasicBlock &BasicBlock : Function.Blocks()) {
      auto [Iterator, Success] = Prefixes.try_emplace(BasicBlock.ID());
      revng_assert(Success, "Duplicate basic blocks?");
      auto &BBPrefixes = Iterator->second;

      for (const yield::Instruction &Instruction : BasicBlock.Instructions()) {
        std::string Address;
        if (!Config.DisableEmissionOfInstructionAddress()) {
          Address = yield::sanitizedAddress(Instruction.Address(), Binary);
          if (llvm::StringRef(Address).take_front(2) == "0x")
            Address = Address.substr(2);
          LongestAddressString = std::max(LongestAddressString, Address.size());
        }

        std::string Bytes;
        if (!Config.DisableEmissionOfRawBytes()) {
          for (uint8_t Byte : Instruction.RawBytes()) {
            std::string HexByte = Byte ? llvm::utohexstr(Byte, true, 2) : "00";
            revng_assert(HexByte.size() == 2);
            Bytes += HexByte + ' ';
          }
          LongestByteString = std::max(LongestByteString, Bytes.size());
        }

        InstructionPrefix Result = { .Address = std::move(Address),
                                     .Bytes = std::move(Bytes) };

        auto [_, Success] = BBPrefixes.try_emplace(Instruction.Address(),
                                                   std::move(Result));
        revng_assert(Success, "Duplicate instructions?");
      }
    }
  }

public:
  /// \note This consumes the internal strings.
  ///       Make sure to only call once per instruction.
  std::string emit(const ptml::MarkupBuilder &B,
                   const MetaAddress &Instruction,
                   const BasicBlockID &BasicBlock,
                   const model::Binary &Binary) {
    if (!LongestAddressString && !LongestByteString)
      return B.getTag(tags::Span, "  ")
        .addAttribute(attributes::Token, ptml::tokens::Indentation)
        .toString();

    InstructionPrefix &Data = Prefixes.at(BasicBlock).at(Instruction);

    std::string Result;
    if (LongestAddressString != 0) {
      Result = B.getTag(tags::Span, std::move(Data.Address))
                 .addAttribute(attributes::Token,
                               tokenTypes::InstructionAddress)
                 .toString();

      revng_assert(Data.Address.size() != 0);
      revng_assert(Data.Address.size() <= LongestAddressString);
      if (Data.Address.size() < LongestAddressString) {
        std::string Indentation(LongestAddressString - Data.Address.size(),
                                ' ');
        Result = B.getTag(tags::Span, std::move(Indentation))
                   .addAttribute(attributes::Token, ptml::tokens::Indentation)
                   .toString()
                 + std::move(Result);
      }

      using model::Architecture::getAssemblyLabelIndicator;
      std::string Indicator(getAssemblyLabelIndicator(Binary.Architecture()));
      Result += B.getTag(tags::Span, std::move(Indicator))
                  .addAttribute(attributes::Token,
                                tokenTypes::InstructionAddress)
                  .toString();
      Result += B.getTag(tags::Span, std::string(4, ' '))
                  .addAttribute(attributes::Token, ptml::tokens::Indentation)
                  .toString();
    }

    if (LongestByteString != 0) {
      Result += B.getTag(tags::Span, std::move(Data.Bytes))
                  .addAttribute(attributes::Token, tokenTypes::RawBytes)
                  .toString();

      revng_assert(Data.Bytes.size() != 0);
      revng_assert(Data.Bytes.size() <= LongestByteString);
      std::string Indentation(LongestByteString + 3 - Data.Bytes.size(), ' ');
      Result += B.getTag(tags::Span, std::move(Indentation))
                  .addAttribute(attributes::Token, ptml::tokens::Indentation)
                  .toString();
    }

    return B.getTag(tags::Span, "  ")
             .addAttribute(attributes::Token, ptml::tokens::Indentation)
             .toString()
           + std::move(Result);
  }

  uint64_t totalPrefixSize(const model::Binary &Binary) const {
    uint64_t Result = 2;

    if (LongestAddressString != 0) {
      using model::Architecture::getAssemblyLabelIndicator;
      auto Indicator = getAssemblyLabelIndicator(Binary.Architecture());
      Result += LongestAddressString + Indicator.size() + 4;
    }

    if (LongestByteString != 0)
      Result += LongestByteString + 3;

    return Result;
  }

  /// \note This does _not_ consume anything, feel free to call as many times
  ///       as you need.
  std::string emitEmpty(const ptml::MarkupBuilder &B,
                        const model::Binary &Binary) const {
    return B.getTag(tags::Span, std::string(totalPrefixSize(Binary), ' '))
      .addAttribute(attributes::Token, ptml::tokens::Indentation)
      .toString();
  }
};

class CommentManager {
private:
  const ptml::MarkupBuilder *B = nullptr;
  const uint64_t TotalPrefixSize = 0;
  std::string_view CommentIndicator = "";

  const model::Function::TypeOfComments *Comments = nullptr;
  std::unordered_map<MetaAddress, std::vector<const model::ContextualComment *>>
    InstructionToCommentMap = {};
  std::unordered_set<const model::ContextualComment *> PlacedComments = {};

private:
  std::string emitImpl(const std::string_view &Text) const {
    return ptml::freeFormComment(*B,
                                 Text,
                                 CommentIndicator,
                                 TotalPrefixSize,
                                 80);
  }

  std::string emitImpl(const MetaAddress &Instruction,
                       const model::ContextualComment &Comment) const {
    return "\n"
           + ptml::contextualComment(*B,
                                     Instruction,
                                     Comment,
                                     CommentIndicator,
                                     TotalPrefixSize,
                                     80);
  }

  // Because MetaAddress compares by `Type` before it compares by `Address`, we
  // can't use normal `lower_bound`, not before we re-sort the list.
  std::optional<MetaAddress>
  findClosestAddress(const yield::BasicBlock &BasicBlock, MetaAddress Target) {
    auto InstructionToAddress = [](const yield::Instruction &I) {
      return I.Address();
    };

    struct Comparator {
      bool operator()(const MetaAddress &LHS, const MetaAddress &RHS) const {
        return LHS.addressLowerThan(RHS);
      }
    };

    std::set ReSorted = BasicBlock.Instructions()
                        | std::views::transform(InstructionToAddress)
                        | revng::to<std::set<MetaAddress, Comparator>>();
    auto Iterator = ReSorted.lower_bound(Target);
    if (Iterator != ReSorted.end())
      return *Iterator;

    return std::nullopt;
  }

public:
  CommentManager() = default;
  CommentManager(const ptml::MarkupBuilder &B,
                 const uint64_t &TotalPrefixSize,
                 std::string_view CommentIndicator,
                 const model::Function::TypeOfComments &Comments,
                 const yield::Function &Function) :
    B(&B),
    TotalPrefixSize(TotalPrefixSize),
    CommentIndicator(CommentIndicator),
    Comments(&Comments) {

    for (const model::ContextualComment &Comment : Comments) {
      for (const yield::BasicBlock &BasicBlock : Function.Blocks()) {
        if (BasicBlock.contains(Comment.Location())) {
          if (auto Addr = findClosestAddress(BasicBlock, Comment.Location())) {
            auto &InstructionComments = InstructionToCommentMap[*Addr];
            if (not llvm::is_contained(InstructionComments, &Comment))
              InstructionComments.push_back(&Comment);

            PlacedComments.emplace(&Comment);
          }
        }
      }
    }
  }

  std::string emit(const MetaAddress &Instruction) const {
    auto It = InstructionToCommentMap.find(Instruction);
    if (It != InstructionToCommentMap.end()) {
      revng_assert(!It->second.empty());

      std::string Result;
      for (const model::ContextualComment *Comment : It->second)
        Result += emitImpl(Instruction, *Comment);
      return Result;

    } else {
      return "";
    }
  }

  std::string emitOrphans() const {
    revng_assert(Comments->size() >= PlacedComments.size());
    if (Comments->size() == PlacedComments.size())
      return "";

    std::string Result;

    for (const model::ContextualComment &Comment : *Comments)
      if (not llvm::is_contained(PlacedComments, &Comment))
        Result += emitImpl(MetaAddress::invalid(), Comment);

    return Result;
  }
};

static std::string instruction(const ptml::MarkupBuilder &B,
                               const yield::Instruction &Instruction,
                               const yield::BasicBlock &BasicBlock,
                               const yield::Function &Function,
                               const model::Binary &Binary,
                               const CommentManager &CM,
                               InstructionPrefixManager &&Prefixes,
                               bool AddTargets = false) {
  revng_assert(Instruction.verify(true));

  std::string Result = CM.emit(Instruction.Address());

  std::string Prefix = Prefixes.emit(B,
                                     Instruction.Address(),
                                     BasicBlock.ID(),
                                     Binary);

  // Tagged instruction body.
  for (const auto &Directive : Instruction.PrecedingDirectives()) {
    Result += B.getTag(tags::Div,
                       std::move(Prefix) + taggedLine(B, Directive.Tags()))
                .toString();
    Prefix = Prefixes.emitEmpty(B, Binary);
  }

  Result += B.getTag(tags::Div,
                     std::move(Prefix)
                       + taggedLine(B, Instruction.Disassembled()))
              .toString();
  Prefix = Prefixes.emitEmpty(B, Binary);

  for (const auto &Directive : Instruction.FollowingDirectives()) {
    Result += B.getTag(tags::Div,
                       std::move(Prefix) + taggedLine(B, Directive.Tags()))
                .toString();
    Prefix = Prefixes.emitEmpty(B, Binary);
  }

  // Tag it with appropriate location data.
  std::string InstructionLocation = locationString(ranks::Instruction,
                                                   Function.Entry(),
                                                   BasicBlock.ID(),
                                                   Instruction.Address());
  Tag Location = B.getTag(tags::Span)
                   .addAttribute(attributes::LocationDefinition,
                                 InstructionLocation);
  Tag Out = B.getTag(tags::Div, std::move(Result))
              .addAttribute(attributes::Scope, scopes::Instruction)
              .addAttribute(attributes::ActionContextLocation,
                            InstructionLocation);

  // And conditionally add target data.
  if (AddTargets) {
    auto Targets = targets(BasicBlock, Function, Binary);
    Out.addListAttribute(attributes::LocationReferences, Targets);
  }

  return Location + Out;
}

static std::string basicBlock(const ptml::MarkupBuilder &B,
                              const yield::BasicBlock &BasicBlock,
                              const yield::Function &Function,
                              const model::Binary &Binary,
                              std::string Label,
                              const CommentManager &CM,
                              InstructionPrefixManager &&Prefixes) {
  revng_assert(!BasicBlock.Instructions().empty());
  auto FromIterator = BasicBlock.Instructions().begin();
  auto ToIterator = std::prev(BasicBlock.Instructions().end());

  std::string Result;
  for (auto Iterator = FromIterator; Iterator != ToIterator; ++Iterator)
    Result += instruction(B,
                          *Iterator,
                          BasicBlock,
                          Function,
                          Binary,
                          CM,
                          std::move(Prefixes));
  Result += instruction(B,
                        *(ToIterator++),
                        BasicBlock,
                        Function,
                        Binary,
                        CM,
                        std::move(Prefixes),
                        true);

  std::string LabelString;
  if (!Label.empty()) {
    LabelString = Label + "\n";
  } else {
    std::string Location = locationString(ranks::BasicBlock,
                                          model::Function(Function.Entry())
                                            .key(),
                                          BasicBlock.ID());
    LabelString = B.getTag(tags::Span)
                    .addAttribute(attributes::LocationDefinition, Location)
                    .toString();
  }

  return B.getTag(tags::Div, LabelString + Result)
    .addAttribute(attributes::Scope, scopes::BasicBlock)
    .toString();
}

template<bool ShouldMergeFallthroughTargets>
static std::string labeledBlock(const ptml::MarkupBuilder &B,
                                const yield::BasicBlock &FirstBlock,
                                const yield::Function &Function,
                                const model::Binary &Binary,
                                const CommentManager &CM = {},
                                InstructionPrefixManager &&Prefixes = {}) {
  std::string Result;
  std::string Label = emitTagged(B, std::move(FirstBlock.Label()));

  using model::Architecture::getAssemblyLabelIndicator;
  std::string Indicator(getAssemblyLabelIndicator(Binary.Architecture()));
  Label += B.getTag(tags::Span, std::move(Indicator))
             .addAttribute(attributes::Token, tokenTypes::LabelIndicator)
             .toString();

  if constexpr (ShouldMergeFallthroughTargets == false) {
    Result = basicBlock(B,
                        FirstBlock,
                        Function,
                        Binary,
                        std::move(Label),
                        CM,
                        std::move(Prefixes))
             + "\n";
  } else {
    auto BasicBlocks = yield::cfg::labeledBlock(FirstBlock, Function, Binary);
    if (BasicBlocks.empty())
      return "";

    bool IsFirst = true;
    for (const auto &BasicBlock : BasicBlocks) {
      Result += basicBlock(B,
                           *BasicBlock,
                           Function,
                           Binary,
                           IsFirst ? std::move(Label) : std::string(),
                           CM,
                           std::move(Prefixes));
      IsFirst = false;
    }
    Result += "\n";
  }

  return Result;
}

std::string yield::ptml::functionAssembly(const ::ptml::MarkupBuilder &B,
                                          const yield::Function &Function,
                                          const model::Binary &Binary) {
  std::string Result;

  InstructionPrefixManager P(Function, Binary);

  const model::Architecture::Values A = Binary.Architecture();
  auto CommentIndicator = model::Architecture::getAssemblyCommentIndicator(A);
  CommentManager CM(B,
                    P.totalPrefixSize(Binary),
                    CommentIndicator,
                    Binary.Functions().at(Function.Entry()).Comments(),
                    Function);

  for (const auto &BBlock : Function.Blocks())
    Result += labeledBlock<true>(B, BBlock, Function, Binary, CM, std::move(P));

  Result += CM.emitOrphans();

  return B.getTag(tags::Div, Result)
    .addAttribute(attributes::Scope, scopes::Function)
    .toString();
}

std::string yield::ptml::controlFlowNode(const ::ptml::MarkupBuilder &B,
                                         const BasicBlockID &BasicBlock,
                                         const yield::Function &Function,
                                         const model::Binary &Binary) {
  auto Iterator = Function.Blocks().find(BasicBlock);
  revng_assert(Iterator != Function.Blocks().end());

  // TODO: enable comments in the CFG too after the size computation rework.
  //
  // CommentManager CM;

  auto Result = labeledBlock<false>(B, *Iterator, Function, Binary);
  revng_assert(!Result.empty());

  return Result;
}

namespace callGraphTokens {

static constexpr auto NodeLabel = "call-graph.node-label";
static constexpr auto ShallowNodeLabel = "call-graph.shallow-node-label";

} // namespace callGraphTokens

static model::Identifier functionNameHelper(std::string_view Location,
                                            const model::Binary &Binary) {
  if (auto L = pipeline::locationFromString(revng::ranks::DynamicFunction,
                                            Location)) {
    auto Key = std::get<0>(L->at(revng::ranks::DynamicFunction));
    auto Iterator = Binary.ImportedDynamicFunctions().find(Key);
    revng_assert(Iterator != Binary.ImportedDynamicFunctions().end());
    return Iterator->name();
  } else if (auto L = pipeline::locationFromString(revng::ranks::Function,
                                                   Location)) {
    auto Key = std::get<0>(L->at(revng::ranks::Function));
    auto Iterator = Binary.Functions().find(Key);
    revng_assert(Iterator != Binary.Functions().end());
    return Iterator->name();
  } else {
    revng_abort("Unsupported function type.");
  }
}

std::string yield::ptml::functionNameDefinition(const ::ptml::MarkupBuilder &B,
                                                std::string_view Location,
                                                const model::Binary &Binary) {
  if (Location.empty())
    return "";

  ::ptml::Tag Result = B.getTag(tags::Div,
                                functionNameHelper(Location, Binary));
  Result.addAttribute(attributes::Token, callGraphTokens::NodeLabel);
  Result.addAttribute(attributes::LocationDefinition, Location);
  return Result.toString();
}

std::string yield::ptml::functionLink(const ::ptml::MarkupBuilder &B,
                                      std::string_view Location,
                                      const model::Binary &Binary) {
  if (Location.empty())
    return "";

  ::ptml::Tag Result = B.getTag(tags::Div,
                                functionNameHelper(Location, Binary));
  Result.addAttribute(attributes::Token, callGraphTokens::NodeLabel);
  Result.addAttribute(attributes::LocationReferences, Location);
  return Result.toString();
}

std::string yield::ptml::shallowFunctionLink(const ::ptml::MarkupBuilder &B,
                                             std::string_view Location,
                                             const model::Binary &Binary) {
  if (Location.empty())
    return "";

  ::ptml::Tag Result = B.getTag(tags::Div,
                                functionNameHelper(Location, Binary));
  Result.addAttribute(attributes::Token, callGraphTokens::ShallowNodeLabel);
  Result.addAttribute(attributes::LocationReferences, Location);
  return Result.toString();
}
