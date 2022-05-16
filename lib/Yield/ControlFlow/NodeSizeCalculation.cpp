/// \file NodeSizeCalculation.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Yield/ControlFlow/Configuration.h"
#include "revng/Yield/ControlFlow/FallthroughDetection.h"
#include "revng/Yield/ControlFlow/NodeSizeCalculation.h"
#include "revng/Yield/Internal/Function.h"
#include "revng/Yield/Internal/Graph.h"

static yield::Graph::Size
operator+(const yield::Graph::Size &LHS, const yield::Graph::Size &RHS) {
  return yield::Graph::Size(LHS.W + RHS.W, LHS.H + RHS.H);
}

constexpr static yield::Graph::Size textSize(std::string_view Text) {
  size_t LineCount = 0;
  size_t MaximumLineLength = 0;

  size_t PreviousPosition = 0;
  size_t CurrentPosition = Text.find('\n');
  while (CurrentPosition != std::string_view::npos) {
    size_t CurrentLineLength = CurrentPosition - PreviousPosition;
    if (CurrentLineLength > MaximumLineLength)
      MaximumLineLength = CurrentLineLength;
    ++LineCount;

    PreviousPosition = CurrentPosition;
    CurrentPosition = Text.find('\n', CurrentPosition + 1);
  }

  size_t LastLineLength = Text.size() - PreviousPosition;
  if (LastLineLength > MaximumLineLength)
    MaximumLineLength = LastLineLength;
  if (LastLineLength != 0)
    ++LineCount;

  return yield::Graph::Size(MaximumLineLength, LineCount);
}

static yield::Graph::Size
fontSize(yield::Graph::Size &&Input,
         yield::Graph::Dimension FontSize,
         const yield::cfg::Configuration &Configuration) {
  Input.W *= FontSize * Configuration.HorizontalFontFactor;
  Input.H *= FontSize * Configuration.VerticalFontFactor;
  return std::move(Input);
}

static yield::Graph::Size
fontSize(yield::Graph::Size &&Input,
         yield::Graph::Dimension HorizontalFontSize,
         yield::Graph::Dimension VerticalFontSize,
         const yield::cfg::Configuration &Configuration) {
  Input.W *= HorizontalFontSize * Configuration.HorizontalFontFactor;
  Input.H *= VerticalFontSize * Configuration.VerticalFontFactor;
  return std::move(Input);
}

static yield::Graph::Size
singleLineSize(std::string_view Text,
               float FontSize,
               const yield::cfg::Configuration &Configuration) {
  yield::Graph::Size Result = fontSize(textSize(Text), FontSize, Configuration);

  Result.W += Configuration.HorizontalInstructionMarginSize * 2;
  Result.H += Configuration.VerticalInstructionMarginSize * 2;

  return Result;
}

static yield::Graph::Size
linkSize(const MetaAddress &Address,
         const model::Binary &Binary,
         size_t IndicatorSize = 0,
         const MetaAddress &NextAddress = MetaAddress::invalid()) {
  yield::Graph::Size Indicator(IndicatorSize, 0);

  auto Iterator = Binary.Functions.find(Address);
  if (Iterator != Binary.Functions.end())
    return Indicator + textSize(Iterator->name().str().str());
  else if (NextAddress == Address)
    return Indicator + textSize("the next instruction");
  else
    return Indicator + textSize("basic_block_at_" + Address.toString());
}

static yield::Graph::Size &
appendSize(yield::Graph::Size &Original, const yield::Graph::Size &AddOn) {
  if (AddOn.W > Original.W)
    Original.W = AddOn.W;
  Original.H += AddOn.H;

  return Original;
}

static yield::Graph::Size
instructionSize(const yield::Instruction &Instruction,
                const yield::cfg::Configuration &Configuration,
                size_t CommentIndicatorSize) {
  // Instruction body.
  yield::Graph::Size Result = fontSize(textSize(Instruction.Raw),
                                       Configuration.InstructionFontSize,
                                       Configuration);

  // Comment.
  if (!Instruction.Comment.empty()) {
    yield::Graph::Size CommentSize = textSize(Instruction.Comment);
    revng_assert(CommentSize.H == 1, "Multi line comments are not supported.");
    CommentSize.W += CommentIndicatorSize + 1;

    const auto FontSize = Configuration.CommentFontSize;
    Result.W += fontSize(std::move(CommentSize), FontSize, Configuration).W;
  }

  // Error.
  if (!Instruction.Error.empty())
    appendSize(Result,
               fontSize(textSize(Instruction.Error)
                          + yield::Graph::Size(CommentIndicatorSize + 1, 0),
                        Configuration.CommentFontSize,
                        Configuration));

  // Instruction address.
  appendSize(Result,
             fontSize(textSize(Instruction.Address.toString())
                        + yield::Graph::Size(CommentIndicatorSize + 1, 0),
                      Configuration.InstructionAddressFontSize,
                      Configuration));

  // Raw instruction bytes.
  appendSize(Result,
             fontSize(yield::Graph::Size(Instruction.Bytes.size() * 3
                                           + CommentIndicatorSize,
                                         1),
                      Configuration.InstructionBytesFontSize,
                      Configuration));

  // Account for the padding
  Result.W += Configuration.HorizontalInstructionMarginSize * 2;
  Result.H += Configuration.VerticalInstructionMarginSize * 2;

  return Result;
}

static size_t countTargets(const SortedVector<MetaAddress> &Targets) {
  return Targets.size() - Targets.count(MetaAddress::invalid());
}

static yield::Graph::Size
targetFooterSize(const yield::BasicBlock &BasicBlock,
                 const model::Binary &Binary,
                 const yield::cfg::Configuration &Configuration) {
  size_t TargetCount = countTargets(BasicBlock.Targets);
  if (TargetCount == 0)
    return yield::Graph::Size(0, 0);

  yield::Graph::Size Result = fontSize(textSize(BasicBlock.CommentIndicator),
                                       Configuration.InstructionFontSize,
                                       Configuration);

  yield::Graph::Size PrefixSize(BasicBlock.CommentIndicator.size() + 1, 0);
  if (BasicBlock.Targets.size() == 1) {
    auto LinkSize = linkSize(*BasicBlock.Targets.begin(),
                             Binary,
                             PrefixSize.W + 1,
                             BasicBlock.NextAddress);
    LinkSize.W += textSize("always goes to ").W;

    return appendSize(Result,
                      fontSize(std::move(LinkSize),
                               Configuration.CommentFontSize,
                               Configuration.InstructionFontSize,
                               Configuration));
  }

  for (const MetaAddress &Target : BasicBlock.Targets)
    if (Target.isValid())
      appendSize(Result,
                 fontSize(linkSize(Target,
                                   Binary,
                                   PrefixSize.W + 3,
                                   BasicBlock.NextAddress),
                          Configuration.CommentFontSize,
                          Configuration.InstructionFontSize,
                          Configuration));

  appendSize(Result,
             fontSize(textSize("known targets include: ") + PrefixSize,
                      Configuration.CommentFontSize,
                      Configuration.InstructionFontSize,
                      Configuration));

  if (BasicBlock.Targets.size() != TargetCount)
    appendSize(Result,
               fontSize(textSize("and more") + PrefixSize,
                        Configuration.CommentFontSize,
                        Configuration.InstructionFontSize,
                        Configuration));

  return Result;
}

static yield::Graph::Size
basicBlockSize(const yield::BasicBlock &BasicBlock,
               const yield::Function &Function,
               const model::Binary &Binary,
               const yield::cfg::Configuration &Configuration) {
  size_t LabelIndicatorSize = BasicBlock.LabelIndicator.size();
  yield::Graph::Size Result = fontSize(linkSize(BasicBlock.Address,
                                                Binary,
                                                LabelIndicatorSize),
                                       Configuration.LabelFontSize,
                                       Configuration);

  // Account for the instructions.
  for (const auto &Instruction : BasicBlock.Instructions)
    appendSize(Result,
               instructionSize(Instruction,
                               Configuration,
                               BasicBlock.CommentIndicator.size()));

  // Deal with the fallthrough.
  if (auto NextBlock = yield::cfg::detectFallthrough<false>(BasicBlock,
                                                            Function,
                                                            Binary)) {
    appendSize(Result,
               basicBlockSize(*NextBlock, Function, Binary, Configuration));
  }

  return Result;
}

void yield::cfg::calculateNodeSizes(Graph &Graph,
                                    const yield::Function &Function,
                                    const model::Binary &Binary,
                                    const Configuration &Configuration) {
  for (auto *Node : Graph.nodes()) {
    revng_assert(Node != nullptr);

    if (Node->Address.isValid()) {
      // A normal node.
      if (auto Iter = Function.BasicBlocks.find(Node->Address);
          Iter != Function.BasicBlocks.end()) {
        Node->Size = basicBlockSize(*Iter, Function, Binary, Configuration);
        appendSize(Node->Size, targetFooterSize(*Iter, Binary, Configuration));
      } else if (auto Iterator = Binary.Functions.find(Node->Address);
                 Iterator != Binary.Functions.end()) {
        Node->Size = singleLineSize(Iterator->name().str(),
                                    Configuration.InstructionFontSize,
                                    Configuration);
      } else {
        revng_abort("The value of this node is not a known address");
      }
    } else if (Node == Graph.getEntryNode()) {
      // An entry node.
      Node->Size = { 30, 30 };
    } else {
      // An error node.
      Node->Size = singleLineSize("Error",
                                  Configuration.CommentFontSize,
                                  Configuration);
    }

    Node->Size.W += Configuration.InternalNodeMarginSize * 2;
    Node->Size.H += Configuration.InternalNodeMarginSize * 2;
  }
}
