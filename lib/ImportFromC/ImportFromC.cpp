//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "clang/AST/Comment.h"
#include "clang/AST/CommentVisitor.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnostic.h"

#include "revng/ABI/ModelHelpers.h"
#include "revng/Model/FunctionAttribute.h"
#include "revng/Model/NameBuilder.h"
#include "revng/Model/Processing.h"
#include "revng/PTML/CAttributes.h"
#include "revng/PTML/CBuilder.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Support/Debug.h"

#include "ImportFromC.h"

using namespace model;
using namespace revng;

static constexpr llvm::StringRef InputCFile = "revng-input.c";
static constexpr llvm::StringRef PrimitiveTypeHeader = "primitive-types.h";
static constexpr llvm::StringRef RawABIPrefix = "raw_";

namespace {

// TODO: listing all the field here is pretty nasty, but it will have to do
//       for now.

void preserveMetadata(const model::Function &Old, model::Function &New) {
  New.StackFrame() = Old.StackFrame();
  New.CallSitePrototypes() = Old.CallSitePrototypes();
  New.ExportedNames() = Old.ExportedNames();
  New.Comments() = Old.Comments();
  New.LocalVariables() = Old.LocalVariables();
  New.GotoLabels() = Old.GotoLabels();

  // TODO: don't forget to extend when new fields are added.
}

template<typename EntityType>
void setNameIfNotAutomatic(model::CNameBuilder &NameBuilder,
                           EntityType &Entity,
                           llvm::StringRef Name) {
  if (not NameBuilder.isAutomaticName(Entity, Name))
    Entity.Name() = Name;
  else
    Entity.Name() = "";
}

template<typename ParentType, typename EntityType>
void setNameIfNotAutomatic(model::CNameBuilder &NameBuilder,
                           const ParentType &Parent,
                           EntityType &Entity,
                           llvm::StringRef Name) {
  if (not NameBuilder.isAutomaticName(Parent, Entity, Name))
    Entity.Name() = Name;
  else
    Entity.Name() = "";
}

// WIP!!!!
//
// The following block is llm-generated. It works but it's messy
// (see tests in the next commit for some :enterprise: highlights).
//
// Clang takes care of the doxygen parsing BUT we have to have weird
// work-arounds to flatten the parsed keywords back into simple text.
//
// What we should probably do instead is just grab `\param` and `\returns`
// manually, splitting the commit into pieces based on that.
//
// For the context, clang still takes care of the comments (delta the two
// side-cases: function comments get attached to `_ABI` and enum entries share
// the same comment between them. But both are easy enough to overcome),
// this is about what should we do about their *internals*.
//
// What do you think?
//
// ---
//
// P. S. There's another important discussion to be had here about how we
// *output* comments. Namely, do we care about emitting *correct* doxygen
// more? Or do we care about user freedom more?
//
// Here's an example of something we can currently emit:
// ```cpp
// /// whatever
// /// \param a this
// ///
// /// is still a part of the `\param` in our output, but doxygen sees it as
// /// separate.
// ```
//
// I'm bringing this up because if we want to keep the newlines, we cannot use
// clang's doxygen parser whether we want to or not.
//
// WIP!!!!

/// Strip a prefix from each line of a multi-line string.
/// Empty lines are preserved.  Leading whitespace after the prefix is kept.
static std::string stripPrefixFromEachLine(llvm::StringRef Text,
                                           llvm::StringRef Prefix) {
  std::string Result;
  llvm::SmallVector<llvm::StringRef, 8> Lines;
  Text.split(Lines, '\n');
  for (llvm::StringRef Line : Lines) {
    llvm::StringRef Stripped = Line;
    // Only strip leading whitespace + prefix from the beginning of the line.
    size_t Pos = 0;
    while (Pos < Stripped.size()
           and (Stripped[Pos] == ' ' or Stripped[Pos] == '\t'
                or Stripped[Pos] == '\n' or Stripped[Pos] == '\r'))
      ++Pos;
    if (Stripped.substr(Pos).startswith(Prefix)) {
      Stripped = Stripped.substr(Pos + Prefix.size());
      // Strip one trailing space after the prefix (common in /// comments).
      if (Stripped.startswith(" "))
        Stripped = Stripped.substr(1);
    }
    if (not Result.empty())
      Result += '\n';
    Result += Stripped.str();
  }
  return Result;
}

std::string getCommentText(const clang::Decl *D,
                           const clang::ASTContext &Context) {
  if (not D)
    return "";

  clang::RawComment *RC = Context.getRawCommentForDeclNoCache(D);
  if (not RC)
    return "";

  const clang::SourceManager &SM = Context.getSourceManager();
  clang::StringRef RawText = RC->getRawText(SM);

  llvm::StringRef Text = RawText.trim();
  if (Text.startswith("///")) {
    return stripPrefixFromEachLine(Text, "///");

  } else if (Text.startswith("//")) {
    return stripPrefixFromEachLine(Text, "//");

  } else if (Text.startswith("/*") and Text.endswith("*/")) {
    return Text.substr(2, Text.size() - 4).trim().str();

  } else {
    return Text.str();
  }
}

static bool isHTMLTag(const auto &CommentBlock) {
  return llvm::isa<clang::comments::HTMLStartTagComment>(CommentBlock)
         or llvm::isa<clang::comments::HTMLEndTagComment>(CommentBlock);
}

// Extract text from a comment node by concatenating child text nodes.
static std::string
extractCommentText(const clang::comments::Comment *C,
                   const clang::comments::CommandTraits *Traits = nullptr) {
  if (not C)
    return "";

  // Inline commands (e.g. \ref) have no children; extract args directly.
  if (auto *ICC = llvm::dyn_cast<clang::comments::InlineCommandComment>(C)) {
    std::string Result;
    if (Traits != nullptr) {
      Result += "\\";
      Result += ICC->getCommandName(*Traits).str();
    }
    for (unsigned I = 0; I < ICC->getNumArgs(); ++I) {
      if (not Result.empty())
        Result += " ";
      Result += ICC->getArgText(I).str();
    }
    return Result;
  }

  // HTML start tags (e.g. <b>, <a href="...">).
  if (auto *HSTC = llvm::dyn_cast<clang::comments::HTMLStartTagComment>(C)) {
    std::string Result = "<" + HSTC->getTagName().str();
    for (unsigned I = 0; I < HSTC->getNumAttrs(); ++I) {
      const auto &Attr = HSTC->getAttr(I);
      Result += " " + Attr.Name.str() + "=\"" + Attr.Value.str() + "\"";
    }
    if (HSTC->isSelfClosing())
      Result += "/>";
    else
      Result += ">";
    return Result;
  }

  // HTML end tags (e.g. </b>).
  if (auto *HETC = llvm::dyn_cast<clang::comments::HTMLEndTagComment>(C)) {
    return "</" + HETC->getTagName().str() + ">";
  }

  // Verbatim block lines (children of \code ... \endcode blocks).
  if (auto *BC = llvm::dyn_cast<clang::comments::VerbatimBlockLineComment>(C)) {
    return BC->getText().str();
  }

  std::string Result;
  for (auto It = C->child_begin(); It != C->child_end(); ++It) {
    if (auto *TC = llvm::dyn_cast<clang::comments::TextComment>(*It)) {
      llvm::StringRef Text = TC->getText().trim();
      if (Text.empty())
        continue;
      if (not Result.empty())
        Result += " ";
      Result += Text.str();
    } else {
      std::string ChildText = extractCommentText(*It, Traits);
      if (not ChildText.empty()) {
        // Don't add spaces around HTML tags to preserve formatting like
        // <b>bold</b> instead of <b> bold </b>.
        bool CurrIsHTMLTag = isHTMLTag(*It);
        bool PrevIsHTMLTag = (It != C->child_begin()) and isHTMLTag(*(It - 1));
        if (not Result.empty() and not PrevIsHTMLTag and not CurrIsHTMLTag)
          Result += " ";
        Result += ChildText;
      }
    }
  }
  return Result;
}

struct FunctionComments {
  std::string Main;
  std::string Return;
  std::map<std::string, std::string> Params;
};

// Internal visitor to extract Doxygen \param and \returns from parsed comments.
struct DoxygenExtractor
  : public clang::comments::ConstCommentVisitor<DoxygenExtractor> {
  const clang::comments::CommandTraits &Traits;
  FunctionComments &Out;
  bool HasSeenBlockCommand = false;

  DoxygenExtractor(const clang::comments::CommandTraits &Traits,
                   FunctionComments &Out) :
    Traits(Traits), Out(Out) {}

  void visitParamCommandComment(const clang::comments::ParamCommandComment *C) {
    if (C->hasParamName()) {
      std::string Name = C->getParamNameAsWritten().str();

      revng_assert(not Out.Params.contains(Name));
      Out.Params[Name] = extractCommentText(C->getParagraph(), &Traits);
    }
  }

  void visitBlockCommandComment(const clang::comments::BlockCommandComment *C) {
    namespace cc = clang::comments;

    HasSeenBlockCommand = true;
    llvm::StringRef CommandName = C->getCommandName(Traits);
    if (CommandName == "returns" or CommandName == "return") {
      revng_assert(Out.Return.empty());
      Out.Return = extractCommentText(C->getParagraph(), &Traits);

    } else {
      std::string Text;
      if (auto *VBC = llvm::dyn_cast<cc::VerbatimBlockComment>(C)) {
        // \code ... \endcode: reconstruct the block with opening command,
        // each line, and closing command.
        Text = "\\" + CommandName.str();
        for (unsigned I = 0; I < VBC->getNumLines(); ++I) {
          Text += "\n";
          Text += VBC->getText(I).str();
        }
        Text += "\n\\" + VBC->getCloseName().str();

      } else if (auto *VLC = llvm::dyn_cast<cc::VerbatimLineComment>(C)) {
        Text = "\\" + CommandName.str();
        llvm::StringRef VerbatimText = VLC->getText().trim();
        if (not VerbatimText.empty()) {
          Text += " ";
          Text += VerbatimText.str();
        }

      } else {
        Text = "\\" + CommandName.str();
        std::string Paragraph = extractCommentText(C->getParagraph(), &Traits);
        if (not Paragraph.empty()) {
          Text += " ";
          Text += Paragraph;
        }
      }
      if (not Text.empty())
        appendToMain(Text);
    }
  }

  void visitParagraphComment(const clang::comments::ParagraphComment *C) {
    std::string Text = extractCommentText(C, &Traits);
    // Trim whitespace
    size_t Start = Text.find_first_not_of(" \t\n\r");
    if (Start == std::string::npos) {
      // Empty or whitespace-only paragraph: add a blank line only if we
      // haven't seen any block commands yet (i.e., it's the blank line
      // separating the main description from block commands).
      if (not HasSeenBlockCommand)
        appendToMain("");
      return;
    }
    size_t End = Text.find_last_not_of(" \t\n\r");
    if (End != std::string::npos)
      Text = Text.substr(Start, End - Start + 1);
    appendToMain(Text);
  }

  void visitFullComment(const clang::comments::FullComment *C) {
    for (auto It = C->child_begin(); It != C->child_end(); ++It)
      visit(*It);
  }

  void visitComment(const clang::comments::Comment *C) {
    for (auto It = C->child_begin(); It != C->child_end(); ++It)
      visit(*It);
  }

private:
  void appendToMain(const std::string &Text) {
    if (not Out.Main.empty())
      Out.Main += "\n";
    Out.Main += Text;
  }
};

static bool isBlockCommand(llvm::StringRef CommandName,
                           const clang::comments::CommandTraits &Traits) {
  if (CommandName.empty())
    return false;
  const clang::comments::CommandInfo
    *Info = Traits.getCommandInfoOrNULL(CommandName);
  if (Info == nullptr)
    return false;
  return Info->IsBlockCommand;
}

/// Strip \param and \returns lines (and their continuation) from raw comment
/// text, returning the remaining text suitable for Comments.Main.
static std::string
stripParamAndReturns(llvm::StringRef Text,
                     const clang::comments::CommandTraits &Traits) {
  llvm::SmallVector<llvm::StringRef, 16> Lines;
  Text.split(Lines, '\n');

  std::string Result;
  bool InParam = false;
  for (llvm::StringRef Line : Lines) {
    llvm::StringRef Trimmed = Line.trim();
    if (Trimmed.empty()) {
      // Blank line ends a \param / \returns description.
      InParam = false;
      if (not Result.empty())
        Result += '\n';
      Result += Line.str();
      continue;
    }

    if (Trimmed.startswith("\\") or Trimmed.startswith("@")) {
      llvm::StringRef CommandName = Trimmed.substr(1).take_until([](char C) {
        return clang::isWhitespace(C);
      });
      if (CommandName == "param" or CommandName == "returns"
          or CommandName == "return") {
        InParam = true;
        continue;
      }
      if (isBlockCommand(CommandName, Traits)) {
        InParam = false;
      }
    }

    if (InParam)
      continue;

    if (not Result.empty())
      Result += '\n';
    Result += Line.str();
  }

  return Result;
}

FunctionComments parseFunctionComment(const clang::FunctionDecl *FD,
                                      const clang::ASTContext &Context) {
  FunctionComments Result;

  namespace cc = clang::comments;
  const cc::CommandTraits &Traits = Context.getCommentCommandTraits();

  // Try the standard comment lookup first.
  const clang::RawComment *RC = Context.getRawCommentForDeclNoCache(FD);

  if (RC == nullptr) {
    // Fallback: when a macro/attribute prefix sits between the comment and
    // the function name, getRawCommentForDeclNoCache returns nullptr
    // because Decl::getBeginLoc() points into the prefix.  Scan all comments
    // in the same file and pick the one whose end is closest to (but still
    // before) the function name token.
    clang::SourceLocation NameLoc = FD->getNameInfo().getBeginLoc();
    if (NameLoc.isValid()) {
      const clang::SourceManager &SM = Context.getSourceManager();
      clang::FileID File = SM.getFileID(NameLoc);
      const std::map<unsigned, clang::RawComment *>
        *CommentsInFile = Context.Comments.getCommentsInFile(File);
      if (CommentsInFile) {
        const clang::RawComment *Best = nullptr;
        clang::SourceLocation BestEnd;
        for (const auto &[Offset, C] : *CommentsInFile) {
          clang::SourceLocation End = C->getEndLoc();
          if (SM.isBeforeInTranslationUnit(End, NameLoc)) {
            if (Best == nullptr || SM.isBeforeInTranslationUnit(BestEnd, End)) {
              Best = C;
              BestEnd = End;
            }
          }
        }
        RC = Best;
      }
    }
  }

  if (RC != nullptr) {
    // Use the FullComment AST only to extract \param and \returns.
    clang::comments::FullComment *FC = RC->parse(Context, nullptr, FD);
    DoxygenExtractor Extractor(Traits, Result);
    Extractor.visit(FC);

    // Use the raw comment text as the base for Main, stripping \param and
    // \returns so they don't appear twice (the emitter emits them separately).
    std::string RawText = getCommentText(FD, Context);
    Result.Main = stripParamAndReturns(RawText, Traits);

    // Strip trailing blank lines from Main.  The emitter adds its own
    // structural blank lines between the main comment and \param/\returns,
    // so we must not preserve trailing empty paragraphs in Main.
    while (not Result.Main.empty() and Result.Main.back() == '\n')
      Result.Main.pop_back();
  }

  return Result;
}

} // namespace

namespace clang {
namespace tooling {

class ImportFromC : public ASTConsumer {
public:
  ImportFromC(TupleTree<model::Binary> &Model,
              std::optional<model::TypeDefinition::Key> Type,
              MetaAddress FunctionEntry,
              ImportingErrorList &Errors,
              enum ImportFromCOption AnalysisOption) :
    Model(Model),
    Type(Type),
    FunctionEntry(FunctionEntry),
    Errors(Errors),
    AnalysisOption(AnalysisOption) {
    // Either one of these two should be null, since the editing features are
    // exclusive.
    revng_assert(not Type or not FunctionEntry.isValid());
  }

  virtual void HandleTranslationUnit(ASTContext &Context) override;

private:
  TupleTree<model::Binary> &Model;
  std::optional<model::TypeDefinition::Key> Type;
  MetaAddress FunctionEntry;
  ImportingErrorList &Errors;
  enum ImportFromCOption AnalysisOption;
};

class DeclVisitor : public clang::RecursiveASTVisitor<DeclVisitor> {
private:
  TupleTree<model::Binary> &Model;
  ASTContext &Context;
  std::optional<model::TypeDefinition::Key> Type;
  MetaAddress FunctionEntry;
  ImportingErrorList &Errors;
  enum ImportFromCOption AnalysisOption;

  // These are used for reporting source location of an error, if any.
  unsigned CurrentLineNumber = 0;
  unsigned CurrentColumnNumber = 0;

  // Used to remember return values locations when parsing struct representing
  // the multi-reg return value. Represents register ID and model::Type.
  using RawLocation = std::pair<model::Register::Values, model::UpcastableType>;
  std::optional<llvm::SmallVector<RawLocation, 4>> MultiRegisterReturnValue;

  model::CNameBuilder NameBuilder;

public:
  DeclVisitor(TupleTree<model::Binary> &Model,
              ASTContext &Context,
              std::optional<model::TypeDefinition::Key> Type,
              MetaAddress FunctionEntry,
              ImportingErrorList &Errors,
              enum ImportFromCOption AnalysisOption);

  void run(clang::TranslationUnitDecl *TUD);
  bool TraverseDecl(clang::Decl *D);

  bool VisitFunctionDecl(const clang::FunctionDecl *FD);
  bool VisitRecordDecl(const clang::RecordDecl *RD);
  bool VisitEnumDecl(const EnumDecl *D);
  bool VisitTypedefDecl(const TypedefDecl *D);
  bool VisitFunctionPrototype(const FunctionProtoType *FP,
                              llvm::StringRef TheABI,
                              const TypedefDecl *D);

private:
  // This checks that the declaration is the one user provided as input.
  bool comesFromInternalFile(const clang::Decl *D);

  // This checks that the declaration comes from primitive-types.header
  // file.
  bool comesFromPrimitiveTypesHeader(const clang::RecordDecl *RD);

  // Set up line and column for the declaratrion.
  void setupLineAndColumn(const clang::Decl *D);

  // Handle clang's Struct type.
  bool handleStructType(const clang::RecordDecl *RD);
  // Handle clang's Union type.
  bool handleUnionType(const clang::RecordDecl *RD);

  // Convert clang::type to model::type.
  model::UpcastableType makePrimitive(const BuiltinType *UnderlyingBuiltin,
                                      QualType Type);

  // Get model type for clang::RecordType (Struct/Unoion).
  model::UpcastableType
  getTypeForRecordType(const clang::RecordType *RecordType,
                       const QualType &ClangType);

  // Get model type for clang::EnumType.
  model::UpcastableType getTypeForEnumType(const clang::EnumType *EnumType);

  template<NonBaseDerived<model::TypeDefinition> T>
  model::UpcastableType makeTypeByNameOrID(llvm::StringRef Name);

  RecursiveCoroutine<model::UpcastableType>
  getModelTypeForClangType(const QualType &QT);

  template<ConstexprString Macro, typename Type>
  std::optional<llvm::StringRef>
  parseStringAnnotation(const Type &Declaration, ImportingErrorList &Errors);

  template<ConstexprString Macro, typename Type>
  std::optional<uint64_t>
  parseIntegerAnnotation(const Type &Declaration, ImportingErrorList &Errors);
};

DeclVisitor::DeclVisitor(TupleTree<model::Binary> &Model,
                         ASTContext &Context,
                         std::optional<model::TypeDefinition::Key> Type,
                         MetaAddress FunctionEntry,
                         ImportingErrorList &Errors,
                         enum ImportFromCOption AnalysisOption) :
  Model(Model),
  Context(Context),
  Type(Type),
  FunctionEntry(FunctionEntry),
  Errors(Errors),
  AnalysisOption(AnalysisOption),
  NameBuilder(*Model) {
}

template<ConstexprString Macro, typename Type>
std::optional<llvm::StringRef>
DeclVisitor::parseStringAnnotation(const Type &Declaration,
                                   ImportingErrorList &Errors) {
  static constexpr auto Prefix = ptml::AttributeRegistry::getPrefix<Macro>();

  std::optional<llvm::StringRef> Result;
  if (Declaration.template hasAttr<clang::AnnotateAttr>()) {
    for (auto &Attribute : Declaration.getAttrs()) {
      if (auto *Cast = llvm::dyn_cast<clang::AnnotateAttr>(Attribute)) {
        llvm::StringRef Annotation = Cast->getAnnotation();
        if (not Annotation.startswith(Prefix))
          continue;

        llvm::StringRef Value = Annotation.substr(Prefix.size());
        if (Result.has_value() && Result.value() != Value) {
          std::string ErrorPrefix = "import-from-c:";

          SourceManager &SM = Context.getSourceManager();
          PresumedLoc Loc = SM.getPresumedLoc(Attribute->getRange().getBegin());
          if (Loc.isValid())
            ErrorPrefix += std::to_string(Loc.getLine()) + ":"
                           + std::to_string(Loc.getColumn()) + ":";

          Errors.emplace_back(ErrorPrefix + " Multiple conflicting values (`"
                              + Result.value().str() + "` and `" + Value.str()
                              + "`) were found for the `" + std::string(Macro)
                              + "` annotation.\n");
          return std::nullopt;
        }

        Result = Value;
      }
    }
  }

  return Result;
}

template<ConstexprString Macro, typename Type>
std::optional<uint64_t>
DeclVisitor::parseIntegerAnnotation(const Type &Declaration,
                                    ImportingErrorList &Errors) {
  std::optional Result = parseStringAnnotation<Macro>(Declaration, Errors);
  if (not Result.has_value())
    return std::nullopt;

  uint64_t IntegerResult;
  if (Result->getAsInteger(0, IntegerResult)) {
    Errors.emplace_back("import-from-c: Ignoring a non-integer value (`"
                        + Result->str() + "`) of an integer annotation: `"
                        + std::string(Macro) + "`.\n");
    return std::nullopt;
  }

  return IntegerResult;
}

static model::Architecture::Values getRawABIArchitecture(llvm::StringRef ABI) {
  revng_assert(ABI.starts_with(RawABIPrefix));
  return model::Architecture::fromName(ABI.substr(RawABIPrefix.size()));
}

model::UpcastableType
DeclVisitor::makePrimitive(const BuiltinType *UnderlyingBuiltin,
                           QualType Type) {
  revng_assert(UnderlyingBuiltin);

  auto AsElaboratedType = Type->getAs<ElaboratedType>();
  if (not AsElaboratedType) {
    PrintingPolicy Policy(Context.getLangOpts());
    Errors.emplace_back("import-from-c: Builtin type `"
                        + UnderlyingBuiltin->getName(Policy).str()
                        + "` not allowed, please use a revng "
                          "model::PrimitiveType instead.\n");

    return model::UpcastableType::empty();
  }

  while (auto Typedef = AsElaboratedType->getAs<TypedefType>()) {
    auto TheUnderlyingType = Typedef->getDecl()->getUnderlyingType();
    if (not TheUnderlyingType->getAs<ElaboratedType>())
      break;
    AsElaboratedType = TheUnderlyingType->getAs<ElaboratedType>();
  }

  std::string TypeName = AsElaboratedType->getNamedType().getAsString();
  if (model::PrimitiveType::fromCName(TypeName).isEmpty()) {
    Errors.emplace_back("import-from-c: `"
                        + AsElaboratedType->getNamedType().getAsString()
                        + "` type is not supported, please use a revng "
                          "model::PrimitiveType instead.\n");

    return model::UpcastableType::empty();
  }

  switch (UnderlyingBuiltin->getKind()) {
  case BuiltinType::UInt128:
    return model::PrimitiveType::makeUnsigned(16);

  case BuiltinType::Int128:
    return model::PrimitiveType::makeSigned(16);

  case BuiltinType::ULongLong:
  case BuiltinType::ULong:
    return model::PrimitiveType::makeUnsigned(8);

  case BuiltinType::LongLong:
  case BuiltinType::Long:
    return model::PrimitiveType::makeSigned(8);

  case BuiltinType::WChar_U:
  case BuiltinType::UInt:
    return model::PrimitiveType::makeUnsigned(4);

  case BuiltinType::WChar_S:
  case BuiltinType::Char32:
  case BuiltinType::Int:
    return model::PrimitiveType::makeSigned(4);

  case BuiltinType::UShort:
    return model::PrimitiveType::makeUnsigned(2);

  case BuiltinType::Char16:
  case BuiltinType::Short:
    return model::PrimitiveType::makeSigned(2);

  case BuiltinType::Char_U:
  case BuiltinType::UChar:
  case BuiltinType::Char8:
  case BuiltinType::Bool:
    return model::PrimitiveType::makeUnsigned(1);

  case BuiltinType::Char_S:
  case BuiltinType::SChar:
    return model::PrimitiveType::makeSigned(1);

  case BuiltinType::Void:
    return model::PrimitiveType::makeVoid();

  case BuiltinType::Float16:
    return model::PrimitiveType::makeFloat(2);

  case BuiltinType::Float:
    return model::PrimitiveType::makeFloat(4);

  case BuiltinType::Double:
    return model::PrimitiveType::makeFloat(8);

  case BuiltinType::Float128:
  case BuiltinType::LongDouble:
    return model::PrimitiveType::makeFloat(16);

  default:
    Errors.emplace_back("import-from-c: Unable to handle a primitive type.\n");
  }

  return model::UpcastableType::empty();
}

static bool onlyContainsNumbers(llvm::StringRef Name) {
  for (char Character : Name)
    if (not std::isdigit(Character))
      return false;
  return true;
}

template<NonBaseDerived<model::TypeDefinition> T>
model::UpcastableType DeclVisitor::makeTypeByNameOrID(llvm::StringRef Name) {
  // Try to find by name first.
  for (auto &Type : Model->TypeDefinitions())
    if (llvm::isa<T>(Type.get()))
      if (Type->Name() == Name)
        return Model->makeType(Type->key());

  // Getting here means we didn't manage to find it,
  // let's try parsing the name.
  size_t Tail = Name.rfind("_");

  if (Tail != std::string::npos && onlyContainsNumbers(Name.substr(Tail + 1))) {
    std::string ID = std::string(Name.substr(Tail + 1));
    llvm::Expected<uint64_t> MaybeID = fromString<uint64_t>(ID);
    if (MaybeID) {
      return Model->makeType(model::TypeDefinition::Key{ *MaybeID,
                                                         T::AssociatedKind });
    } else {
      Errors.emplace_back(consumeToString(MaybeID));
    }
  }

  return model::UpcastableType::empty();
}

model::UpcastableType
DeclVisitor::getTypeForRecordType(const clang::RecordType *RecordType,
                                  const QualType &ClangType) {
  revng_assert(RecordType);

  // Check if it is a primitive type described with a struct.
  if (comesFromPrimitiveTypesHeader(RecordType->getDecl())) {
    const TypedefType *AsTypedef = ClangType->getAs<TypedefType>();
    if (not AsTypedef) {
      Errors.emplace_back("import-from-c: There should be a typedef for struct "
                          "that defines the primitive type.\n");
      return model::UpcastableType::empty();
    }
    auto TypeName = AsTypedef->getDecl()->getName();
    auto R = model::PrimitiveType::fromCName(TypeName);
    revng_assert(R);
    return R;
  }

  auto Name = RecordType->getDecl()->getName();
  if (Name.empty()) {
    Errors.emplace_back("import-from-c: Nameless structs and unions are not "
                        "supported here, since we have no way to trace them "
                        "back to one of the types present in the model.\n");
    return model::UpcastableType::empty();
  }

  if (RecordType->isStructureType()) {
    if (auto Struct = makeTypeByNameOrID<model::StructDefinition>(Name))
      return Struct;

  } else if (RecordType->isUnionType()) {
    if (auto Union = makeTypeByNameOrID<model::UnionDefinition>(Name))
      return Union;
  }

  Errors.emplace_back("import-from-c: Unknown struct or union: `" + Name.str()
                      + "`.\n");
  return model::UpcastableType::empty();
}

model::UpcastableType
DeclVisitor::getTypeForEnumType(const clang::EnumType *EnumType) {
  revng_assert(EnumType);
  revng_assert(AnalysisOption != ImportFromCOption::EditFunctionPrototype);

  auto EnumName = EnumType->getDecl()->getName();
  if (EnumName.empty()) {
    Errors.emplace_back("import-from-c: Nameless enums are not supported here, "
                        "since we have no way to trace them back to one of the "
                        "types present in the model.\n");
    return model::UpcastableType::empty();
  }

  if (auto Enum = makeTypeByNameOrID<model::EnumDefinition>(EnumName))
    return Enum;

  Errors.emplace_back("import-from-c: Unknown enum: `" + EnumName.str()
                      + "`.\n");
  return model::UpcastableType::empty();
}

bool DeclVisitor::comesFromInternalFile(const clang::Decl *D) {
  SourceManager &SM = Context.getSourceManager();
  PresumedLoc Loc = SM.getPresumedLoc(D->getLocation());
  if (!Loc.isValid())
    return false;

  StringRef TheFileName(Loc.getFilename());
  // Process the new type only.
  if (TheFileName.contains(InputCFile))
    return true;

  return false;
}

bool DeclVisitor::comesFromPrimitiveTypesHeader(const clang::RecordDecl *RD) {
  SourceManager &SM = Context.getSourceManager();
  PresumedLoc Loc = SM.getPresumedLoc(RD->getLocation());
  if (!Loc.isValid())
    return false;

  StringRef TheFileName(Loc.getFilename());
  if (TheFileName.contains(PrimitiveTypeHeader))
    return true;

  return false;
}

void DeclVisitor::setupLineAndColumn(const clang::Decl *D) {
  SourceManager &SM = Context.getSourceManager();
  PresumedLoc Loc = SM.getPresumedLoc(D->getLocation());
  if (!Loc.isValid())
    return;

  CurrentLineNumber = Loc.getLine();
  CurrentColumnNumber = Loc.getColumn();
}

RecursiveCoroutine<model::UpcastableType>
DeclVisitor::getModelTypeForClangType(const QualType &QT) {
  model::UpcastableType R;

  if (const BuiltinType *AsBuiltinType = QT->getAs<BuiltinType>()) {
    R = makePrimitive(AsBuiltinType, QT);

  } else if (const PointerType *Pointer = QT->getAs<PointerType>()) {
    QualType Pointee = Pointer->getPointeeType();
    R = model::PointerType::make(rc_recur getModelTypeForClangType(Pointee),
                                 Model->Architecture());

  } else if (QT->isArrayType()) {
    if (const auto *CAT = dyn_cast<ConstantArrayType>(QT)) {
      QualType ElementType = Context.getBaseElementType(QT);
      uint64_t NumberOfElements = CAT->getSize().getZExtValue();
      R = model::ArrayType::make(rc_recur getModelTypeForClangType(ElementType),
                                 NumberOfElements);
    } else {
      // Here we can face `clang::VariableArrayType` and
      // `clang::IncompleteArrayType`.
      Errors.emplace_back("import-from-c: Unsupported type used as an "
                          "array.\n");
    }
  } else if (const RecordType *AsRecordType = QT->getAs<RecordType>()) {
    R = getTypeForRecordType(AsRecordType, QT);

  } else if (const EnumType *AsEnum = QT->getAs<EnumType>()) {
    R = getTypeForEnumType(AsEnum);

  } else if (const auto *AsFn = QT->getAs<FunctionProtoType>()) {
    if (const TypedefType *AsTypedef = QT->getAs<TypedefType>()) {
      auto Name = AsTypedef->getDecl()->getName();
      if (auto CFT = makeTypeByNameOrID<model::CABIFunctionDefinition>(Name))
        R = std::move(CFT);
      else if (auto Rw = makeTypeByNameOrID<model::RawFunctionDefinition>(Name))
        R = std::move(Rw);
      else
        Errors.emplace_back("import-from-c: Unknown typedef: `" + Name.str()
                            + "`.\n");
    } else {
      Errors.emplace_back("import-from-c: Model has to contain a typedef for "
                          "the function prototype.\n");
    }

  } else {
    Errors.emplace_back("import-from-c: The type cannot be represented in the "
                        "model.\n");
  }

  if (not R.isEmpty() and QT.isConstQualified())
    R->IsConst() = true;

  rc_return R;
}

bool DeclVisitor::VisitFunctionDecl(const clang::FunctionDecl *FD) {
  if (not comesFromInternalFile(FD))
    return true;

  revng_assert(FD);
  revng_assert(AnalysisOption == ImportFromCOption::EditFunctionPrototype);

  std::optional ABI = parseStringAnnotation<"_ABI">(*FD, Errors);
  if (not ABI.has_value() or ABI->empty()) {
    Errors.emplace_back("import-from-c failed: Functions without an "
                        "`_ABI($name)` or `_ABI(raw_$arch)` annotation are not "
                        "allowed.\n");
    return false;
  }

  bool IsRawFunctionType = ABI->starts_with(RawABIPrefix);
  auto NewType = IsRawFunctionType ?
                   makeTypeDefinition<RawFunctionDefinition>() :
                   makeTypeDefinition<CABIFunctionDefinition>();

  if (not IsRawFunctionType) {
    auto TheModelABI = model::ABI::fromName(*ABI);
    if (TheModelABI == model::ABI::Invalid) {
      Errors.emplace_back("import-from-c failed: Unknown ABI: `" + ABI->str()
                          + "`.\n");
      return false;
    }

    auto &FunctionType = llvm::cast<CABIFunctionDefinition>(*NewType);
    FunctionType.ABI() = TheModelABI;
    auto TheRetClangType = FD->getReturnType();
    model::UpcastableType RetType = getModelTypeForClangType(TheRetClangType);
    if (not RetType) {
      Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                          "the return value.\n");
      return false;
    }

    FunctionType.ReturnType() = std::move(RetType);

    // Handle params.
    uint32_t Index = 0;
    for (unsigned I = 0, N = FD->getNumParams(); I != N; ++I) {
      auto QT = FD->getParamDecl(I)->getType();
      model::UpcastableType ParamType = getModelTypeForClangType(QT);
      if (not ParamType) {
        Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                            "the argument #"
                            + std::to_string(I) + ".\n");
        return false;
      }

      model::Argument &NewArgument = FunctionType.Arguments()[Index];

      setNameIfNotAutomatic(NameBuilder,
                            FunctionType,
                            NewArgument,
                            FD->getParamDecl(I)->getName());

      NewArgument.Type() = std::move(ParamType);
      ++Index;
    }
  } else {
    auto TheRetClangType = FD->getReturnType();
    auto &TheRawFunctionType = llvm::cast<RawFunctionDefinition>(*NewType);

    auto Architecture = getRawABIArchitecture(*ABI);
    if (Architecture == model::Architecture::Invalid) {
      Errors.emplace_back("import-from-c failed: Unknown architecture: `"
                          + ABI->substr(RawABIPrefix.size()).str() + "`.\n");
      return false;
    }
    TheRawFunctionType.Architecture() = Architecture;

    auto ReturnValuesInserter = TheRawFunctionType.ReturnValues()
                                  .batch_insert();

    // This represents multiple register location for return values.
    if (TheRetClangType->isStructureType()) {
      if (not MultiRegisterReturnValue) {
        Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                            "the return value.\n");
        return false;
      }

      for (auto &[Location, Type] : *MultiRegisterReturnValue) {
        model::NamedTypedRegister &NTR = ReturnValuesInserter.emplace(Location);
        NTR.Type() = Type;
      }
    } else {
      std::optional Register = parseStringAnnotation<"_REG">(*FD, Errors);
      if (not Register.has_value()) {
        std::optional Stack = parseStringAnnotation<"_STACK">(*FD, Errors);
        if (Stack.has_value()) {
          Errors.emplace_back("import-from-c failed: Only register values are "
                              "allowed as a part of a raw function's return "
                              "value. As such, they must not use _STACK "
                              "annotation.\n");
          return false;

        } else {
          Errors.emplace_back("import-from-c failed: Return values of a raw "
                              "function must have a _REG($name) annotation.\n");
          return false;
        }
      }

      model::UpcastableType RetType = getModelTypeForClangType(TheRetClangType);
      if (not RetType) {
        Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                            "the return value.\n");
        return false;
      }

      auto Location = model::Register::fromRegisterName(*Register,
                                                        Model->Architecture());
      if (Location == model::Register::Invalid) {
        Errors.emplace_back("import-from-c: While parsing the return value:\n");
        Errors.emplace_back("import-from-c failed: Unknown register: `"
                            + Register->str() + "`.\n");
        return false;
      }

      auto &ReturnValueReg = ReturnValuesInserter.emplace(Location);
      ReturnValueReg.Type() = std::move(RetType);
    }

    auto ArgumentsInserter = TheRawFunctionType.Arguments().batch_insert();
    for (unsigned I = 0, N = FD->getNumParams(); I != N; ++I) {
      auto ParamDecl = FD->getParamDecl(I);
      auto QT = ParamDecl->getType();
      model::UpcastableType ParamType = getModelTypeForClangType(QT);
      if (not ParamType) {
        Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                            "the argument #"
                            + std::to_string(I) + ".\n");
        return false;
      }

      std::optional Register = parseStringAnnotation<"_REG">(*ParamDecl,
                                                             Errors);
      std::optional Stack = parseStringAnnotation<"_STACK">(*ParamDecl, Errors);
      if (not Register.has_value()) {
        if (not Stack.has_value()) {
          Errors.emplace_back("import-from-c failed: Argument #"
                              + std::to_string(I)
                              + " is missing it's location annotation.\n");
          Errors.emplace_back("                      Please add either "
                              "`_REG($name)` or `_STACK`.\n");
          return false;
        } else {
          if (I != N - 1) {
            Errors.emplace_back("import-from-c failed: Only the very last RFT "
                                "argument is allowed to represent stack, which "
                                "also means there can only be one.\n");
            Errors.emplace_back("                      Please either remove "
                                "`_STACK` annotation from the argument #"
                                + std::to_string(I)
                                + " or move it into the stack argument "
                                  "struct.\n");
            return false;
          }

          if (not ParamType->isStruct()) {
            Errors.emplace_back("import-from-c failed: RFT stack argument must "
                                "be a "
                                "struct. You can use fields of such a struct "
                                "to represent separate arguments.\n");
            return false;
          }

          revng_assert(TheRawFunctionType.StackArgumentsType().isEmpty());

          TheRawFunctionType.StackArgumentsType() = std::move(ParamType);
          if (ParamDecl->getName() != "stack") {
            Errors.emplace_back("import-from-c: stack argument name (`"
                                + ParamDecl->getName().str()
                                + "`) was ignored, as model stores the struct "
                                  "as is.\n");
          }
        }
      } else {
        if (Stack.has_value()) {
          Errors.emplace_back("import-from-c failed: A single argument cannot "
                              "use both a register and stack: the model does "
                              "not support that. Please use two separate "
                              "arguments.\n");
          return false;
        }

        using namespace model;
        auto Location = Register::fromRegisterName(*Register,
                                                   Model->Architecture());
        if (Location == Register::Invalid) {
          Errors.emplace_back("import-from-c: While parsing argument #"
                              + std::to_string(I) + ":\n");
          Errors.emplace_back("import-from-c failed: Unknown register: `"
                              + Register->str() + "`.\n");
          return false;
        }

        NamedTypedRegister &ParamReg = ArgumentsInserter.emplace(Location);
        ParamReg.Type() = std::move(ParamType);
        if (not ParamDecl->getName().empty())
          setNameIfNotAutomatic(NameBuilder,
                                TheRawFunctionType,
                                ParamReg,
                                ParamDecl->getName());
      }
    }
  }

  // Update the name in case it changed.
  auto &ModelFunction = Model->Functions()[FunctionEntry];

  if (FD->hasAttr<clang::NoReturnAttr>()
      || FD->hasAttr<clang::C11NoReturnAttr>()) {
    ModelFunction.Attributes().emplace(model::FunctionAttribute::NoReturn);
  }

  if (FD->hasAttr<clang::AlwaysInlineAttr>())
    ModelFunction.Attributes().emplace(model::FunctionAttribute::AlwaysInline);

  setNameIfNotAutomatic(NameBuilder, ModelFunction, FD->getName());

  if (auto *OriginalFunction = Model->Functions().tryGet(FunctionEntry))
    preserveMetadata(*OriginalFunction, ModelFunction);

  FunctionComments Comments = parseFunctionComment(FD, Context);
  if (not Comments.Main.empty())
    ModelFunction.Comment() = Comments.Main;
  if (not IsRawFunctionType) {
    auto &FunctionType = llvm::cast<CABIFunctionDefinition>(*NewType);
    for (auto &Argument : FunctionType.Arguments()) {
      std::string EffectiveName = NameBuilder.name(FunctionType, Argument);
      if (auto It = Comments.Params.find(EffectiveName);
          It != Comments.Params.end()) {
        Argument.Comment() = It->second;
      }
    }

    FunctionType.ReturnValueComment() = Comments.Return;

  } else {
    auto &FunctionType = llvm::cast<RawFunctionDefinition>(*NewType);
    for (auto &Argument : FunctionType.Arguments()) {
      std::string EffectiveName = NameBuilder.name(FunctionType, Argument);
      if (auto It = Comments.Params.find(EffectiveName);
          It != Comments.Params.end()) {
        Argument.Comment() = It->second;
      }
    }

    FunctionType.ReturnValueComment() = Comments.Return;
  }

  auto &&[_, Prototype] = Model->recordNewType(std::move(NewType));
  ModelFunction.Prototype() = Prototype;

  return true;
}

bool DeclVisitor::VisitTypedefDecl(const TypedefDecl *D) {
  if (not comesFromInternalFile(D))
    return true;

  revng_assert(AnalysisOption != ImportFromCOption::EditFunctionPrototype);

  QualType TheType = D->getUnderlyingType();
  if (auto Fn = llvm::dyn_cast<FunctionProtoType>(TheType)) {
    // Parse the ABI from annotate attribute attached to the typedef
    // declaration. Please do note that annotations on the parameters are not
    // attached, so we will use default RawFunctionDefinition from the Model if
    // the abi is raw.
    // TODO: Should we change the annotate attached to function types to have
    // info about parameters in the toplevel annotate attribute attached to
    // the typedef itself?
    std::optional ABI = parseStringAnnotation<"_ABI">(*D, Errors);
    if (not ABI.has_value()) {
      Errors.emplace_back("import-from-c failed: a function typedef must "
                          "either have `_ABI($name)` or `_ABI(raw_$arch)` "
                          "annotation attached.\n");
      return false;
    }
    if (ABI->empty()) {
      Errors.emplace_back("import-from-c failed: _ABI annotation must not be "
                          "empty.\n");
      Errors.emplace_back("                      Please specify an abi name or "
                          "an architecture name.\n");
      return false;
    }

    return VisitFunctionPrototype(Fn, *ABI, D);
  }

  // Regular, non-function, typedef.
  model::UpcastableType ModelTypedefType = getModelTypeForClangType(TheType);
  if (not ModelTypedefType) {
    Errors.emplace_back("import-from-c failed: Unable to parse the underlying "
                        "type of the typedef.\n");
    return false;
  }
  auto &&[ID, Kind] = *Type;
  auto NewTypedef = model::makeTypeDefinition<model::TypedefDefinition>();
  if (AnalysisOption == ImportFromCOption::EditType)
    NewTypedef->ID() = ID;

  auto TheTypeTypeDef = cast<model::TypedefDefinition>(NewTypedef.get());
  TheTypeTypeDef->UnderlyingType() = std::move(ModelTypedefType);

  setNameIfNotAutomatic(NameBuilder, *TheTypeTypeDef, D->getName());
  if (std::string Text = getCommentText(D, Context); not Text.empty())
    TheTypeTypeDef->Comment() = std::move(Text);

  if (AnalysisOption == ImportFromCOption::EditType) {
    revng_assert(*Type == NewTypedef->key());
    Model->TypeDefinitions().erase(*Type);
    Model->TypeDefinitions().insert(std::move(NewTypedef));
  } else {
    Model->recordNewType(std::move(NewTypedef));
  }

  return true;
}

bool DeclVisitor::VisitFunctionPrototype(const FunctionProtoType *FP,
                                         llvm::StringRef ABI,
                                         const TypedefDecl *D) {
  revng_assert(AnalysisOption != ImportFromCOption::EditFunctionPrototype);
  revng_assert(ABI != "");

  bool IsRawFunctionType = ABI.starts_with(RawABIPrefix);
  auto NewType = IsRawFunctionType ?
                   makeTypeDefinition<RawFunctionDefinition>() :
                   makeTypeDefinition<CABIFunctionDefinition>();

  auto &&[ID, Kind] = *Type;
  if (AnalysisOption == ImportFromCOption::EditType)
    NewType->ID() = ID;

  if (not IsRawFunctionType) {
    auto &FunctionType = llvm::cast<CABIFunctionDefinition>(*NewType);
    auto TheModelABI = model::ABI::fromName(ABI);
    if (TheModelABI == model::ABI::Invalid) {
      Errors.emplace_back("import-from-c failed: Unknown ABI: `" + ABI.str()
                          + "`.\n");
      return false;
    }

    FunctionType.ABI() = TheModelABI;

    auto TheRetClangType = FP->getReturnType();
    model::UpcastableType RetType = getModelTypeForClangType(TheRetClangType);
    if (not RetType) {
      Errors.emplace_back("import-from-c failed: Unable to parse the return "
                          "value type.\n");
      return false;
    }

    FunctionType.ReturnType() = std::move(RetType);

    // Handle params.
    uint32_t Index = 0;
    for (auto QT : FP->getParamTypes()) {
      model::UpcastableType ParamType = getModelTypeForClangType(QT);
      if (not ParamType) {
        Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                            "argument #`"
                            + std::to_string(Index) + "`.\n");
        return false;
      }

      model::Argument &NewArgument = FunctionType.Arguments()[Index];
      NewArgument.Type() = std::move(ParamType);
      ++Index;
    }
  } else {
    auto Architecture = getRawABIArchitecture(ABI);
    if (Architecture == model::Architecture::Invalid) {
      Errors.emplace_back("import-from-c failed: Unknown architecture: `"
                          + ABI.substr(RawABIPrefix.size()).str() + "`.\n");
      return false;
    }

    auto &FunctionType = cast<RawFunctionDefinition>(*NewType);
    FunctionType.Architecture() = Architecture;

    // TODO: Since we do not have info about parameters annotation, we use
    // default raw function.
    if (AnalysisOption == ImportFromCOption::EditType) {
      // Preserve existing arguments/return values when editing
      if (auto *OldType = Model->TypeDefinitions().tryGet(*Type)) {
        if (auto *OldFn = dyn_cast<RawFunctionDefinition>(&**OldType)) {
          FunctionType.Arguments() = OldFn->Arguments();
          FunctionType.ReturnValues() = OldFn->ReturnValues();
          FunctionType.PreservedRegisters() = OldFn->PreservedRegisters();
          FunctionType.FinalStackOffset() = OldFn->FinalStackOffset();
        }
      }
    } else {
      // For new types, copy from default prototype
      auto *DefaultPrototype = Model->defaultPrototype();
      if (DefaultPrototype) {
        if (auto *Default = dyn_cast<RawFunctionDefinition>(DefaultPrototype)) {
          FunctionType.Arguments() = Default->Arguments();
          FunctionType.ReturnValues() = Default->ReturnValues();
          FunctionType.PreservedRegisters() = Default->PreservedRegisters();
          FunctionType.FinalStackOffset() = Default->FinalStackOffset();
        }
      }
    }
  }

  model::TypeDefinition::Key NewTypeKey = NewType->key();
  if (AnalysisOption == ImportFromCOption::EditType) {
    revng_assert(*Type == NewTypeKey);
    Model->TypeDefinitions().erase(*Type);
    Model->TypeDefinitions().insert(std::move(NewType));
  } else {
    Model->recordNewType(std::move(NewType));
  }

  // For typedef function types, set the name and comment from the typedef
  if (D) {
    model::TypeDefinition::Key Key = *Type;
    if (AnalysisOption != ImportFromCOption::EditType)
      Key = NewTypeKey;

    if (auto *Definition = Model->TypeDefinitions().tryGet(Key)) {
      model::TypeDefinition *Ptr = Definition->get();
      if (auto *CFT = llvm::dyn_cast<model::CABIFunctionDefinition>(Ptr)) {
        setNameIfNotAutomatic(NameBuilder, *CFT, D->getName());
        if (std::string Text = getCommentText(D, Context); not Text.empty())
          CFT->Comment() = std::move(Text);

      } else if (auto *R = llvm::dyn_cast<model::RawFunctionDefinition>(Ptr)) {
        setNameIfNotAutomatic(NameBuilder, *R, D->getName());
        if (std::string Text = getCommentText(D, Context); not Text.empty())
          R->Comment() = std::move(Text);
      }
    }
  }

  return true;
}

bool DeclVisitor::handleStructType(const clang::RecordDecl *RD) {
  const RecordDecl *Definition = RD->getDefinition();
  if (Definition == nullptr) {
    Errors.emplace_back("import-from-c failed: Unable to parse the struct.\n");
    return false;
  }

  auto &&[ID, Kind] = *Type;
  auto NewType = makeTypeDefinition<model::StructDefinition>();
  if (AnalysisOption == ImportFromCOption::EditType)
    NewType->ID() = ID;

  setNameIfNotAutomatic(NameBuilder, *NewType, RD->getName());
  if (std::string Text = getCommentText(RD, Context); not Text.empty())
    NewType->Comment() = std::move(Text);

  auto *Struct = cast<model::StructDefinition>(NewType.get());
  uint64_t CurrentOffset = 0;

  const model::StructDefinition *OldStruct = nullptr;
  if (AnalysisOption == ImportFromCOption::EditType)
    if (auto *OldType = Model->TypeDefinitions().tryGet(*Type))
      OldStruct = dyn_cast<model::StructDefinition>(&**OldType);

  //
  // Iterate over the struct fields
  //
  llvm::SmallVector<RawLocation, 4> ReturnValues;
  for (const FieldDecl *Field : Definition->fields()) {
    if (Field->isInvalidDecl()) {
      Errors.emplace_back("import-from-c failed: The declaration of the struct "
                          "field #`"
                          + std::to_string(Struct->Fields().size())
                          + "` is not valid.\n");
      return false;
    }

    model::Register::Values Location;
    if (AnalysisOption == ImportFromCOption::EditFunctionPrototype) {
      std::optional Stack = parseStringAnnotation<"_STACK">(*Field, Errors);
      if (Stack.has_value()) {
        Errors.emplace_back("import-from-c failed: Only register values are "
                            "allowed as a part of a raw function's return "
                            "value. As such, they must not use _STACK "
                            "annotation.\n");
        return false;
      }

      std::optional Register = parseStringAnnotation<"_REG">(*Field, Errors);
      if (not Register.has_value()) {
        Errors.emplace_back("import-from-c failed: Return values of a raw "
                            "function must have a _REG($name) annotation.\n");
        return false;
      }

      Location = model::Register::fromRegisterName(*Register,
                                                   Model->Architecture());
      if (Location == model::Register::Invalid) {
        Errors.emplace_back("import-from-c: While parsing return value #"
                            + std::to_string(Struct->Fields().size()) + ":\n");
        Errors.emplace_back("import-from-c failed: Unknown register: `"
                            + Register->str() + "`.\n");
        return false;
      }
    }

    std::optional<uint64_t> Size = 0;
    const QualType &ClangFieldType = Field->getType();
    model::UpcastableType ModelField = getModelTypeForClangType(ClangFieldType);

    if (ModelField.isEmpty()) {
      Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                          "struct field #"
                          + std::to_string(Struct->Fields().size()) + ".\n");
      return false;
    }

    if (AnalysisOption == ImportFromCOption::EditFunctionPrototype)
      ReturnValues.emplace_back(Location, ModelField);

    if (ClangFieldType->isPointerType()) {
      Size = model::Architecture::getPointerSize(Model->Architecture());

    } else if (ClangFieldType->isArrayType()) {
      uint64_t NumberOfElements = 0;
      if (const auto *CAT = dyn_cast<ConstantArrayType>(ClangFieldType)) {
        NumberOfElements = CAT->getSize().getZExtValue();
      } else {
        Errors.emplace_back("import-from-c failed: Unsupported array type.\n");
        return false;
      }

      const model::Type &Element = *ModelField->toArray().ElementType();
      Size = *Element.size() * NumberOfElements;

    } else {
      Size = *ModelField->size();
    }

    const auto &Config = Model->Configuration().Naming();
    bool IsPadding = Field->getName().starts_with(Config.StructPaddingPrefix());
    auto ExplicitOffset = parseIntegerAnnotation<"_STARTS_AT">(*Field, Errors);
    if (ExplicitOffset.has_value()) {
      if (IsPadding) {
        Errors.emplace_back("import-from-c: While parsing field #"
                            + std::to_string(Struct->Fields().size()) + ":\n");
        Errors.emplace_back("import-from-c failed: Padding fields (`uint8_t "
                            "padding_at_$offset[$size]`) must not have "
                            "`_STARTS_AT` annotation attached.\n");
        return false;
      }

      if (not Struct->Fields().empty() and CurrentOffset > *ExplicitOffset) {
        Errors.emplace_back("import-from-c: While parsing field #"
                            + std::to_string(Struct->Fields().size()) + ":\n");
        Errors.emplace_back("import-from-c failed: `_STARTS_AT` must not be "
                            "used to make fields overlap.\n");
        return false;
      }

      CurrentOffset = *ExplicitOffset;
    }

    if (not IsPadding) {
      auto &FieldModelType = Struct->Fields()[CurrentOffset];

      setNameIfNotAutomatic(NameBuilder,
                            *Struct,
                            FieldModelType,
                            Field->getName());
      if (std::string Text = getCommentText(Field, Context); not Text.empty())
        FieldModelType.Comment() = std::move(Text);

      FieldModelType.Type() = std::move(ModelField);
    } else {
      // Do not create fields for padding
    }

    revng_assert(Size);
    CurrentOffset += *Size;
  }

  if (std::optional ExplicitSize = parseIntegerAnnotation<"_SIZE">(*Definition,
                                                                   Errors)) {
    // Prefer explicit size if it's available.
    Struct->Size() = *ExplicitSize;

  } else {
    // If not, just use final offset,
    Struct->Size() = CurrentOffset;

    // Unless we're editing a type and have access to the previous size.
    if (OldStruct != nullptr)
      if (auto OldSize = *OldStruct->size(); Struct->Size() < OldSize)
        Struct->Size() = OldSize;
  }

  if (parseStringAnnotation<"_CAN_CONTAIN_CODE">(*RD, Errors))
    Struct->CanContainCode() = true;

  switch (AnalysisOption) {
  case ImportFromCOption::EditType:
    revng_assert(*Type == NewType->key());
    Model->TypeDefinitions().erase(*Type);
    Model->TypeDefinitions().insert(std::move(NewType));
    break;

  case ImportFromCOption::EditFunctionPrototype:
    MultiRegisterReturnValue = std::move(ReturnValues);
    break;

  case ImportFromCOption::AddType:
    Model->recordNewType(std::move(NewType));
    break;
  }

  return true;
}

bool DeclVisitor::handleUnionType(const clang::RecordDecl *RD) {
  revng_assert(AnalysisOption != ImportFromCOption::EditFunctionPrototype);

  const RecordDecl *Definition = RD->getDefinition();
  if (Definition == nullptr) {
    Errors.emplace_back("import-from-c failed: Unable to parse the union.\n");
    return false;
  }

  auto &&[ID, Kind] = *Type;
  auto NewType = makeTypeDefinition<model::UnionDefinition>();
  if (AnalysisOption == ImportFromCOption::EditType)
    NewType->ID() = ID;

  setNameIfNotAutomatic(NameBuilder, *NewType, RD->getName());
  if (std::string Text = getCommentText(RD, Context); not Text.empty())
    NewType->Comment() = std::move(Text);

  auto Union = cast<model::UnionDefinition>(NewType.get());

  uint64_t CurrentIndex = 0;
  for (const FieldDecl *Field : Definition->fields()) {
    if (Field->isInvalidDecl()) {
      Errors.emplace_back("import-from-c failed: The declaration of the union "
                          "field #`"
                          + std::to_string(Union->Fields().size())
                          + "` is not valid.\n");
      return false;
    }

    const QualType &FieldType = Field->getType();
    model::UpcastableType TheFieldType = getModelTypeForClangType(FieldType);

    if (not TheFieldType) {
      Errors.emplace_back("import-from-c failed: Unable to parse the type of "
                          "union field #"
                          + std::to_string(Union->Fields().size()) + ".\n");
      return false;
    }

    auto &FieldModelType = Union->Fields()[CurrentIndex];
    setNameIfNotAutomatic(NameBuilder,
                          *Union,
                          FieldModelType,
                          Field->getName());
    if (std::string Text = getCommentText(Field, Context); not Text.empty())
      FieldModelType.Comment() = std::move(Text);

    FieldModelType.Type() = std::move(TheFieldType);

    ++CurrentIndex;
  }

  if (AnalysisOption == ImportFromCOption::EditType) {
    revng_assert(*Type == NewType->key());
    Model->TypeDefinitions().erase(*Type);
    Model->TypeDefinitions().insert(std::move(NewType));
  } else {
    Model->recordNewType(std::move(NewType));
  }

  return true;
}

bool DeclVisitor::VisitRecordDecl(const clang::RecordDecl *RD) {
  if (not comesFromInternalFile(RD))
    return true;

  if (AnalysisOption != ImportFromCOption::EditFunctionPrototype
      and not RD->hasAttr<PackedAttr>()) {
    Errors.emplace_back("import-from-c failed: Unions and Structs must be "
                        "`_PACKED`.\n");
    return false;
  }

  QualType TheType = Context.getTypeDeclType(RD);
  if (TheType->isStructureType()) {
    return handleStructType(RD);
  } else if (TheType->isUnionType()) {
    return handleUnionType(RD);
  } else {
    Errors.emplace_back("import-from-c failed: As of now, only struct and "
                        "union record types are supported.\n");
    Errors.emplace_back("                      Please rewrite your type as one "
                        "of those two.\n");
    return false;
  }

  return true;
}

bool DeclVisitor::VisitEnumDecl(const EnumDecl *D) {
  if (not comesFromInternalFile(D))
    return true;

  revng_assert(AnalysisOption != ImportFromCOption::EditFunctionPrototype);

  if (not D->hasAttr<PackedAttr>()) {
    Errors.emplace_back("import-from-c failed: Enums must be `_PACKED`.\n");
    return false;
  }

  // Parse annotate attribute used for specifying underlying type.
  auto UnderlyingTypeName = parseStringAnnotation<"_ENUM_UNDERLYING">(*D,
                                                                      Errors);
  if (not UnderlyingTypeName.has_value()) {
    Errors.emplace_back("import-from-c failed: Enums without an "
                        "`_ENUM_UNDERLYING($type)` annotation are not "
                        "allowed.\n");
    return false;
  }
  if (UnderlyingTypeName->empty()) {
    Errors.emplace_back("import-from-c failed: `_ENUM_UNDERLYING` must not be "
                        "empty: please specify a valid type name.\n");
    return false;
  }

  revng_assert(UnderlyingTypeName.has_value());
  auto UnderlyingType = model::PrimitiveType::fromCName(*UnderlyingTypeName);
  if (not UnderlyingType) {
    Errors.emplace_back("import-from-c failed: unknown primitive type: `"
                        + UnderlyingTypeName->str() + "`.\n");
    return false;

  } else if (not UnderlyingType->isSignedPrimitive()
             and not UnderlyingType->isUnsignedPrimitive()) {
    Errors.emplace_back("import-from-c failed: Underlying type of an enum can "
                        "only be signed or unsigned.\n");
    Errors.emplace_back("                      `" + UnderlyingTypeName->str()
                        + "` was found instead.\n");

    return false;
  }

  model::EnumDefinition *NewType = nullptr;
  if (AnalysisOption == ImportFromCOption::EditType) {
    revng_assert(Type != std::nullopt);
    model::TypeDefinition &Definition = *Model->TypeDefinitions().at(*Type);
    if (auto *Enum = llvm::dyn_cast<model::EnumDefinition>(&Definition)) {
      NewType = Enum;
    } else {
      // It seems like the kind of the type got changed. Since it affects
      // the key we need to erase the old type before adding the new one.
      Model->TypeDefinitions().erase(*Type);
      NewType = &Model->makeEnumDefinition().first;
    }
  }

  NewType->UnderlyingType() = std::move(UnderlyingType);

  auto *Definition = D->getDefinition();
  setNameIfNotAutomatic(NameBuilder, *NewType, Definition->getName());
  if (std::string Text = getCommentText(Definition, Context); not Text.empty())
    NewType->Comment() = std::move(Text);

  clang::RawComment *LastEnumComment = nullptr;
  for (const auto *Enum : Definition->enumerators()) {
    auto Value = Enum->getInitVal().getExtValue();

    // Skip the auto-generated max value entry
    const auto Prefix = NameBuilder.Configuration.MaximumEnumValuePrefix();
    if (Enum->getName().starts_with(Prefix))
      continue;

    auto NewIterator = NewType->Entries().insert(Value).first;
    setNameIfNotAutomatic(NameBuilder,
                          *NewType,
                          *NewIterator,
                          Enum->getName().str());

    // Only set comment if it's different from the last one (prevents
    // propagation to consecutive entries separated by commas)
    clang::RawComment *RC = Context.getRawCommentForDeclNoCache(Enum);
    if (RC != LastEnumComment) {
      if (std::string Text = getCommentText(Enum, Context); not Text.empty()) {
        NewIterator->Comment() = std::move(Text);
        LastEnumComment = RC;
      } else {
        LastEnumComment = nullptr;
      }
    }
  }

  return true;
}

void DeclVisitor::run(clang::TranslationUnitDecl *TUD) {
  this->TraverseDecl(TUD);
}

bool DeclVisitor::TraverseDecl(clang::Decl *D) {
  // This can happen due to an error in the code.
  if (!D)
    return true;

  setupLineAndColumn(D);

  clang::RecursiveASTVisitor<DeclVisitor>::TraverseDecl(D);
  return true;
}

void ImportFromC::HandleTranslationUnit(ASTContext &Context) {
  clang::TranslationUnitDecl *TUD = Context.getTranslationUnitDecl();
  DeclVisitor(Model, Context, Type, FunctionEntry, Errors, AnalysisOption)
    .run(TUD);
}

std::unique_ptr<ASTConsumer> ImportFromCEditTypeAction::newASTConsumer() {
  return std::make_unique<ImportFromC>(Model,
                                       Type,
                                       MetaAddress::invalid(),
                                       Errors,
                                       AnalysisOption);
}

std::unique_ptr<ASTConsumer> ImportFromCEditFunctionAction::newASTConsumer() {
  return std::make_unique<ImportFromC>(Model,
                                       std::nullopt,
                                       FunctionEntry,
                                       Errors,
                                       AnalysisOption);
}

std::unique_ptr<ASTConsumer> ImportFromCAddTypeAction::newASTConsumer() {
  return std::make_unique<ImportFromC>(Model,
                                       std::nullopt,
                                       MetaAddress::invalid(),
                                       Errors,
                                       AnalysisOption);
}

std::unique_ptr<ASTConsumer>
ImportFromCAction::CreateASTConsumer(CompilerInstance &, llvm::StringRef) {
  return newASTConsumer();
}

bool ImportFromCAction::BeginInvocation(clang::CompilerInstance &CI) {
  DiagConsumer = new ImportFromCDiagnosticConsumer(CI.getDiagnostics());
  CI.getDiagnostics().setClient(DiagConsumer, false);
  return true;
}

void ImportFromCAction::EndSourceFile() {
  if (DiagConsumer) {
    for (auto &Error : DiagConsumer->extractErrors())
      Errors.emplace_back(std::move(Error));
  }
}

void ImportFromCDiagnosticConsumer::EndSourceFile() {
  Client->EndSourceFile();
}

using Level = DiagnosticsEngine::Level;
void ImportFromCDiagnosticConsumer::HandleDiagnostic(Level DiagLevel,
                                                     const Diagnostic &Info) {
  SmallString<100> OutStr;
  Info.FormatDiagnostic(OutStr);

  llvm::raw_svector_ostream DiagMessageStream(OutStr);

  std::string Text;
  llvm::raw_string_ostream OS(Text);
  auto *DiagOpts = &Info.getDiags()->getDiagnosticOptions();

  uint64_t StartOfLocationInfo = OS.tell();

  TextDiagnostic::printDiagnosticLevel(OS, DiagLevel, DiagOpts->ShowColors);
  const bool IsSupplemental = DiagLevel == DiagnosticsEngine::Note;
  TextDiagnostic::printDiagnosticMessage(OS,
                                         IsSupplemental,
                                         DiagMessageStream.str(),
                                         OS.tell() - StartOfLocationInfo,
                                         DiagOpts->MessageLength,
                                         DiagOpts->ShowColors);
  OS.flush();

  unsigned Line = 0;
  unsigned Column = 0;
  std::string FileName;
  if (Info.getLocation().isValid()) {
    FullSourceLoc Location(Info.getLocation(), Info.getSourceManager());
    Line = Location.getLineNumber();
    Column = Location.getColumnNumber();
  }

  // Report all the messages coming from clang.
  Errors.emplace_back("clang:" + std::to_string(Line) + ":"
                      + std::to_string(Column) + ": " + std::move(Text));
}

} // end namespace tooling
} // end namespace clang
