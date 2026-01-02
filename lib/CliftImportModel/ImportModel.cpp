//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <type_traits>

#include "llvm/ADT/SmallSet.h"
#include "llvm/Support/FormatVariadic.h"

#include "mlir/IR/BuiltinOps.h"

#include "revng/ADT/RecursiveCoroutine.h"
#include "revng/Clift/Clift.h"
#include "revng/Clift/CliftAttributes.h"
#include "revng/Clift/CliftDialect.h"
#include "revng/Clift/CliftTypes.h"
#include "revng/Clift/Helpers.h"
#include "revng/Clift/ModuleVisitor.h"
#include "revng/CliftImportModel/AttributeHelpers.h"
#include "revng/CliftImportModel/ImportModel.h"
#include "revng/Model/CABIFunctionDefinition.h"
#include "revng/Model/Function.h"
#include "revng/Model/NameBuilder.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Support/Identifier.h"

namespace {

namespace clift = mlir::clift;

template<typename Attribute>
using AttributeVector = llvm::SmallVector<Attribute, 16>;

class CliftConverter {
  mlir::MLIRContext *Context;
  llvm::function_ref<mlir::InFlightDiagnostic()> EmitError;

  llvm::DenseMap<uint64_t, clift::DefinedType> Cache;
  llvm::DenseMap<uint64_t, const model::TypeDefinition *> IncompleteTypes;

  llvm::SmallSet<uint64_t, 16> DefinitionGuardSet;

  class RecursiveDefinitionGuard {
    CliftConverter *Self = nullptr;
    uint64_t ID;

  public:
    explicit RecursiveDefinitionGuard(CliftConverter &Self, const uint64_t ID) {
      if (Self.DefinitionGuardSet.insert(ID).second) {
        this->Self = &Self;
        this->ID = ID;
      }
    }

    RecursiveDefinitionGuard(const RecursiveDefinitionGuard &) = delete;
    RecursiveDefinitionGuard &
    operator=(const RecursiveDefinitionGuard &) = delete;

    ~RecursiveDefinitionGuard() {
      if (Self != nullptr) {
        size_t const Erased = Self->DefinitionGuardSet.erase(ID);
        revng_assert(Erased == 1);
      }
    }

    explicit operator bool() const { return Self != nullptr; }
  };

public:
  explicit CliftConverter(mlir::MLIRContext &Context,
                          llvm::function_ref<mlir::InFlightDiagnostic()>
                            EmitError) :
    Context(&Context), EmitError(EmitError) {}

  CliftConverter(const CliftConverter &) = delete;
  CliftConverter &operator=(const CliftConverter &) = delete;

  ~CliftConverter() { revng_assert(DefinitionGuardSet.empty()); }

  clift::ValueType
  convertTypeDefinition(const model::TypeDefinition &ModelType) {
    const clift::ValueType T = fromTypeDefinition(ModelType,
                                                  /* RequireComplete = */ true);
    if (T and not processIncompleteTypes())
      return nullptr;
    return T;
  }

  clift::ValueType convertType(const model::Type &ModelType) {
    const clift::ValueType T = fromType(ModelType,
                                        /* RequireComplete = */ true);
    if (T and not processIncompleteTypes())
      return nullptr;
    return T;
  }

private:
  template<typename T, typename... ArgTypes>
  T make(const ArgTypes &...Args) {
    return T::getChecked(EmitError, Context, Args...);
  }

  template<typename KeyT>
  clift::MutableStringAttr
  makeNameAttr(llvm::StringRef Handle, llvm::StringRef Name = {}) {
    return clift::makeNameAttr<KeyT>(Context, Handle, Name);
  }
  template<typename KeyT>
  clift::MutableStringAttr
  makeCommentAttr(llvm::StringRef Handle, llvm::StringRef Comment = {}) {
    return clift::makeCommentAttr<KeyT>(Context, Handle, Comment);
  }
  template<typename KeyT>
  clift::MutableStringAttr
  makeRVCommentAttr(llvm::StringRef Handle, llvm::StringRef Comment = {}) {
    return clift::makeRVCommentAttr<KeyT>(Context, Handle, Comment);
  }

  static clift::PrimitiveKind
  getPrimitiveKind(const model::PrimitiveType &ModelType) {
    switch (ModelType.PrimitiveKind()) {
    case model::PrimitiveKind::Void:
      return clift::PrimitiveKind::VoidKind;
    case model::PrimitiveKind::Generic:
      return clift::PrimitiveKind::GenericKind;
    case model::PrimitiveKind::PointerOrNumber:
      return clift::PrimitiveKind::PointerOrNumberKind;
    case model::PrimitiveKind::Number:
      return clift::PrimitiveKind::NumberKind;
    case model::PrimitiveKind::Unsigned:
      return clift::PrimitiveKind::UnsignedKind;
    case model::PrimitiveKind::Signed:
      return clift::PrimitiveKind::SignedKind;
    case model::PrimitiveKind::Float:
      return clift::PrimitiveKind::FloatKind;

    case model::PrimitiveKind::Invalid:
    case model::PrimitiveKind::Count:
      revng_abort("These are invalid values. Something has gone wrong.");
    }
  }

  auto getLocation(const model::TypeDefinition &T) {
    return pipeline::location(revng::ranks::TypeDefinition, T.key());
  }

  std::string getHandle(const model::TypeDefinition &T) {
    return getLocation(T).toString();
  }

  std::string getRegisterSetHandle(const model::RawFunctionDefinition &T) {
    return pipeline::locationString(revng::ranks::ArtificialStruct, T.key());
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::CABIFunctionDefinition &ModelType) {
    RecursiveDefinitionGuard Guard(*this, ModelType.ID());
    if (not Guard) {
      if (EmitError)
        EmitError() << "Recursive definition of CABIFunctionDefinition "
                    << ModelType.ID();
      rc_return nullptr;
    }

    AttributeVector<mlir::Type> ArgumentTypes;
    ArgumentTypes.reserve(ModelType.Arguments().size());

    for (const model::Argument &Argument : ModelType.Arguments()) {
      const auto Type = rc_recur fromType(*Argument.Type());
      if (not Type)
        rc_return nullptr;
      ArgumentTypes.push_back(Type);
    }

    mlir::Type ReturnType = nullptr;
    if (ModelType.ReturnType().isEmpty())
      ReturnType = rc_recur fromType(*model::PrimitiveType::makeVoid());
    else
      ReturnType = rc_recur fromType(*ModelType.ReturnType());

    if (not ReturnType)
      rc_return nullptr;

    auto ABI = mlir::clift::setAttribute<"_ABI">(Context,
                                                 toString(ModelType.ABI()));

    auto Handle = getHandle(ModelType);
    auto NameAttr = makeNameAttr<clift::FunctionType>(Handle);
    auto CommentAttr = makeCommentAttr<clift::FunctionType>(Handle);
    auto RVCommentAttr = makeRVCommentAttr<clift::FunctionType>(Handle);
    rc_return make<clift::FunctionType>(llvm::StringRef(Handle),
                                        NameAttr,
                                        CommentAttr,
                                        RVCommentAttr,
                                        ReturnType,
                                        llvm::ArrayRef(ArgumentTypes),
                                        ABI);
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::EnumDefinition &ModelType) {
    RecursiveDefinitionGuard Guard(*this, ModelType.ID());
    if (not Guard) {
      if (EmitError)
        EmitError() << "Recursive definition of EnumDefinition "
                    << ModelType.ID();
      rc_return nullptr;
    }

    const auto UnderlyingType = rc_recur fromType(*ModelType.UnderlyingType());
    if (not UnderlyingType)
      rc_return nullptr;

    auto Location = getLocation(ModelType);

    AttributeVector<clift::EnumFieldAttr> Fields;
    Fields.reserve(ModelType.Entries().size());

    for (const model::EnumEntry &Entry : ModelType.Entries()) {
      auto Handle = Location.extend(revng::ranks::EnumEntry, Entry.Value())
                      .toString();

      auto NameAttr = makeNameAttr<clift::EnumFieldAttr>(Handle);
      auto CommentAttr = makeCommentAttr<clift::EnumFieldAttr>(Handle);
      auto Attr = make<clift::EnumFieldAttr>(llvm::StringRef(Handle),
                                             NameAttr,
                                             CommentAttr,
                                             Entry.Value());

      if (not Attr)
        rc_return nullptr;

      Fields.push_back(Attr);
    }

    auto Handle = Location.toString();
    auto NameAttr = makeNameAttr<clift::EnumAttr>(Handle);
    auto CommentAttr = makeCommentAttr<clift::EnumAttr>(Handle);
    auto Attr = make<clift::EnumAttr>(llvm::StringRef(Handle),
                                      NameAttr,
                                      CommentAttr,
                                      UnderlyingType,
                                      llvm::ArrayRef(Fields));

    if (not Attr)
      rc_return nullptr;

    rc_return clift::EnumType::get(Attr);
  }

  RecursiveCoroutine<clift::ValueType>
  getRegisterSetType(const model::RawFunctionDefinition &ModelType) {
    auto Location = getLocation(ModelType);

    AttributeVector<clift::FieldAttr> Fields;
    Fields.reserve(ModelType.ReturnValues().size());

    uint64_t Offset = 0;
    for (const model::NamedTypedRegister &Register : ModelType.ReturnValues()) {
      const auto RegisterType = rc_recur fromType(*Register.Type());
      if (not RegisterType)
        rc_return nullptr;

      auto Handle = Location
                      .extend(revng::ranks::ReturnRegister, Register.Location())
                      .toString();

      auto NameAttr = makeNameAttr<clift::FieldAttr>(Handle);
      auto CommentAttr = makeCommentAttr<clift::FieldAttr>(Handle);
      auto Attr = make<clift::FieldAttr>(llvm::StringRef(Handle),
                                         NameAttr,
                                         CommentAttr,
                                         Offset,
                                         RegisterType);
      if (not Attr)
        rc_return nullptr;

      Fields.push_back(Attr);
      Offset += RegisterType.getByteSize();
    }

    auto Handle = Location.transmute(revng::ranks::ArtificialStruct).toString();
    auto NameAttr = makeNameAttr<clift::StructAttr>(Handle);
    auto CommentAttr = makeCommentAttr<clift::StructAttr>(Handle);
    auto Attr = make<clift::StructAttr>(llvm::StringRef(Handle),
                                        NameAttr,
                                        CommentAttr,
                                        Offset,
                                        llvm::ArrayRef(Fields),
                                        mlir::ArrayAttr::get(Context, {}));

    if (not Attr)
      rc_return nullptr;

    rc_return clift::StructType::get(Attr);
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::RawFunctionDefinition &ModelType) {
    RecursiveDefinitionGuard Guard(*this, ModelType.ID());
    if (not Guard) {
      if (EmitError)
        EmitError() << "Recursive definition of RawFunctionDefinition "
                    << ModelType.ID();
      rc_return nullptr;
    }

    mlir::Type StackArgumentType;
    size_t ArgumentsCount = 0;

    if (not ModelType.StackArgumentsType().isEmpty()) {
      StackArgumentType = rc_recur fromType(*ModelType.StackArgumentsType());
      if (not StackArgumentType)
        rc_return nullptr;
      ++ArgumentsCount;
    }

    ArgumentsCount += ModelType.Arguments().size();
    AttributeVector<mlir::Type> ArgumentTypes;
    ArgumentTypes.reserve(ArgumentsCount);

    for (const model::NamedTypedRegister &Register : ModelType.Arguments()) {
      const auto Type = rc_recur fromType(*Register.Type());
      if (not Type)
        rc_return nullptr;
      ArgumentTypes.push_back(Type);
    }

    if (StackArgumentType)
      ArgumentTypes.push_back(StackArgumentType);

    clift::ValueType ReturnType;
    switch (ModelType.ReturnValues().size()) {
    case 0:
      ReturnType = make<clift::PrimitiveType>(clift::PrimitiveKind::VoidKind,
                                              /*Size=*/static_cast<uint64_t>(0),
                                              /*IsConst=*/false);
      break;

    case 1:
      ReturnType = rc_recur fromType(*ModelType.ReturnValues().begin()->Type());
      break;

    default: {
      ReturnType = clift::StructType::get(Context,
                                          getRegisterSetHandle(ModelType));

      const auto R = IncompleteTypes.try_emplace(ModelType.ID(), &ModelType);
      revng_assert(R.second && "Register set types are only visited once.");
    } break;
    }
    if (not ReturnType)
      rc_return nullptr;

    std::string RawABI = "raw_" + toString(ModelType.Architecture());
    auto ABI = mlir::clift::setAttribute<"_ABI">(Context, RawABI);

    auto Handle = getHandle(ModelType);
    auto NameAttr = makeNameAttr<clift::FunctionType>(Handle);
    auto CommentAttr = makeCommentAttr<clift::FunctionType>(Handle);
    auto RVCommentAttr = makeCommentAttr<clift::FunctionType>(Handle);
    rc_return make<clift::FunctionType>(llvm::StringRef(Handle),
                                        NameAttr,
                                        CommentAttr,
                                        RVCommentAttr,
                                        mlir::Type(ReturnType),
                                        llvm::ArrayRef(ArgumentTypes),
                                        ABI);
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::StructDefinition &ModelType,
                    const bool RequireComplete) {
    if (not RequireComplete) {
      const auto T = clift::StructType::get(Context, getHandle(ModelType));
      if (not T.isComplete())
        IncompleteTypes.try_emplace(ModelType.ID(), &ModelType);
      rc_return T;
    }

    RecursiveDefinitionGuard Guard(*this, ModelType.ID());
    if (not Guard) {
      if (EmitError)
        EmitError() << "Recursive definition of StructTypeAttr "
                    << ModelType.ID();
      rc_return nullptr;
    }

    auto Location = getLocation(ModelType);

    AttributeVector<clift::FieldAttr> Fields;
    Fields.reserve(ModelType.Fields().size());

    for (const model::StructField &Field : ModelType.Fields()) {
      const auto FieldType = rc_recur fromType(*Field.Type(),
                                               /* RequireComplete = */ true);
      if (not FieldType)
        rc_return nullptr;

      auto Handle = Location.extend(revng::ranks::StructField, Field.Offset())
                      .toString();

      auto NameAttr = makeNameAttr<clift::FieldAttr>(Handle);
      auto CommentAttr = makeCommentAttr<clift::FieldAttr>(Handle);
      auto Attr = make<clift::FieldAttr>(llvm::StringRef(Handle),
                                         NameAttr,
                                         CommentAttr,
                                         Field.Offset(),
                                         FieldType);
      if (not Attr)
        rc_return nullptr;

      Fields.push_back(Attr);
    }

    mlir::ArrayAttr Attributes = mlir::ArrayAttr::get(Context, {});
    if (ModelType.CanContainCode())
      Attributes = mlir::clift::setAttribute<"_CAN_CONTAIN_CODE">(Context);

    auto Handle = Location.toString();
    auto NameAttr = makeNameAttr<clift::StructAttr>(Handle);
    auto CommentAttr = makeCommentAttr<clift::StructAttr>(Handle);
    auto Attr = make<clift::StructAttr>(llvm::StringRef(Handle),
                                        NameAttr,
                                        CommentAttr,
                                        ModelType.Size(),
                                        llvm::ArrayRef(Fields),
                                        Attributes);

    if (not Attr)
      rc_return nullptr;

    rc_return clift::StructType::get(Attr);
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::TypedefDefinition &ModelType,
                    const bool RequireComplete) {
    std::optional<RecursiveDefinitionGuard> Guard;

    if (RequireComplete) {
      Guard.emplace(*this, ModelType.ID());
      if (not *Guard) {
        if (EmitError)
          EmitError() << "Recursive definition of TypedefDefinition "
                      << ModelType.ID();
        rc_return nullptr;
      }
    }

    const auto UnderlyingType = rc_recur fromType(*ModelType.UnderlyingType(),
                                                  RequireComplete);
    if (not UnderlyingType)
      rc_return nullptr;

    auto Handle = getHandle(ModelType);
    auto NameAttr = makeNameAttr<clift::TypedefAttr>(Handle);
    auto CommentAttr = makeCommentAttr<clift::TypedefAttr>(Handle);
    auto Attr = make<clift::TypedefAttr>(llvm::StringRef(Handle),
                                         NameAttr,
                                         CommentAttr,
                                         UnderlyingType);

    if (not Attr)
      rc_return nullptr;

    rc_return clift::TypedefType::get(Attr);
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::UnionDefinition &ModelType,
                    const bool RequireComplete) {
    if (not RequireComplete) {
      const auto T = clift::UnionType::get(Context, getHandle(ModelType));
      if (not T.isComplete())
        IncompleteTypes.try_emplace(ModelType.ID(), &ModelType);
      rc_return T;
    }

    RecursiveDefinitionGuard Guard(*this, ModelType.ID());
    if (not Guard) {
      if (EmitError)
        EmitError() << "Recursive definition of UnionTypeAttr "
                    << ModelType.ID();
      rc_return nullptr;
    }

    auto Location = getLocation(ModelType);

    AttributeVector<clift::FieldAttr> Fields;
    Fields.reserve(ModelType.Fields().size());

    for (const model::UnionField &Field : ModelType.Fields()) {
      const auto FieldType = rc_recur fromType(*Field.Type(),
                                               /* RequireComplete = */ true);
      if (not FieldType)
        rc_return nullptr;

      auto Handle = Location.extend(revng::ranks::UnionField, Field.Index())
                      .toString();

      auto NameAttr = makeNameAttr<clift::FieldAttr>(Handle);
      auto CommentAttr = makeCommentAttr<clift::FieldAttr>(Handle);
      auto Attr = make<clift::FieldAttr>(llvm::StringRef(Handle),
                                         NameAttr,
                                         CommentAttr,
                                         /*Offset=*/static_cast<uint64_t>(0),
                                         FieldType);
      if (not Attr)
        rc_return nullptr;
      Fields.push_back(Attr);
    }

    auto Handle = Location.toString();
    auto NameAttr = makeNameAttr<clift::UnionAttr>(Handle);
    auto CommentAttr = makeCommentAttr<clift::UnionAttr>(Handle);
    auto Attr = make<clift::UnionAttr>(llvm::StringRef(Handle),
                                       NameAttr,
                                       CommentAttr,
                                       llvm::ArrayRef(Fields),
                                       mlir::ArrayAttr::get(Context, {}));

    rc_return clift::UnionType::get(Attr);
  }

  RecursiveCoroutine<clift::DefinedType>
  getTypeDefinition(const model::TypeDefinition &T, bool &RequireComplete) {
    if (const auto *CFT = llvm::dyn_cast<model::CABIFunctionDefinition>(&T))
      rc_return getTypeDefinition(*CFT);

    if (const auto *RFT = llvm::dyn_cast<model::RawFunctionDefinition>(&T))
      rc_return getTypeDefinition(*RFT);

    if (const auto *Enum = llvm::dyn_cast<model::EnumDefinition>(&T))
      rc_return getTypeDefinition(*Enum);

    if (const auto *Struct = llvm::dyn_cast<model::StructDefinition>(&T))
      rc_return getTypeDefinition(*Struct, RequireComplete);

    if (const auto *Union = llvm::dyn_cast<model::UnionDefinition>(&T))
      rc_return getTypeDefinition(*Union, RequireComplete);

    if (const auto *Typedef = llvm::dyn_cast<model::TypedefDefinition>(&T))
      rc_return getTypeDefinition(*Typedef, RequireComplete);

    revng_abort("Unsupported type definition kind.");
  }

  RecursiveCoroutine<clift::ValueType>
  fromTypeDefinition(const model::TypeDefinition &ModelType,
                     bool RequireComplete = false,
                     const bool Const = false) {
    if (Const) {
      auto Type = rc_recur fromTypeDefinition(ModelType,
                                              RequireComplete,
                                              /*Const=*/false);
      rc_return Type.addConst();
    }

    if (const auto It = Cache.find(ModelType.ID()); It != Cache.end())
      rc_return It->second;

    if (not ModelType.verify()) {
      if (EmitError)
        EmitError() << "Invalid model type definition";

      rc_return nullptr;
    }

    auto Type = rc_recur getTypeDefinition(ModelType, RequireComplete);

    if (Type and RequireComplete) {
      auto [Iterator, Inserted] = Cache.try_emplace(ModelType.ID(), Type);
      revng_assert(Inserted);
    }

    rc_return Type;
  }

  RecursiveCoroutine<clift::ValueType> fromType(const model::Type &ModelType,
                                                bool RequireComplete = false) {
    if (not ModelType.verify()) {
      if (EmitError)
        EmitError() << "Invalid model type";

      rc_return nullptr;
    }

    if (const auto &P = llvm::dyn_cast<model::PrimitiveType>(&ModelType)) {
      rc_return make<clift::PrimitiveType>(getPrimitiveKind(*P),
                                           P->Size(),
                                           P->IsConst());

    } else if (const auto &D = llvm::dyn_cast<model::DefinedType>(&ModelType)) {
      rc_return fromTypeDefinition(D->unwrap(), RequireComplete, D->IsConst());

    } else if (const auto &A = llvm::dyn_cast<model::ArrayType>(&ModelType)) {
      rc_return make<clift::ArrayType>(rc_recur fromType(*A->ElementType(),
                                                         RequireComplete),
                                       A->ElementCount());

    } else if (const auto &P = llvm::dyn_cast<model::PointerType>(&ModelType)) {
      // If there's a pointer in the way, the base type does not have to be
      // complete.
      RequireComplete = false;

      rc_return make<clift::PointerType>(rc_recur fromType(*P->PointeeType(),
                                                           RequireComplete),
                                         P->PointerSize(),
                                         P->IsConst());

    } else {
      if (EmitError)
        EmitError() << "Unknown model type";

      rc_return nullptr;
    }
  }

  bool processIncompleteTypes() {
    while (not IncompleteTypes.empty()) {
      const auto Iterator = IncompleteTypes.begin();
      const model::TypeDefinition &ModelType = *Iterator->second;
      IncompleteTypes.erase(Iterator);

      clift::ValueType CompleteType;
      if (auto RFT = llvm::dyn_cast<model::RawFunctionDefinition>(&ModelType)) {
        CompleteType = getRegisterSetType(*RFT);
      } else {
        CompleteType = fromTypeDefinition(ModelType, /*RequireComplete=*/true);
      }

      if (not CompleteType)
        return false;
    }

    return true;
  }
};

} // namespace

clift::ValueType
clift::importModelType(llvm::function_ref<mlir::InFlightDiagnostic()> EmitError,
                       mlir::MLIRContext &Context,
                       const model::TypeDefinition &ModelType) {
  return CliftConverter(Context, EmitError).convertTypeDefinition(ModelType);
}

clift::ValueType
clift::importModelType(llvm::function_ref<mlir::InFlightDiagnostic()> EmitError,
                       mlir::MLIRContext &Context,
                       const model::Type &ModelType) {
  return CliftConverter(Context, EmitError).convertType(ModelType);
}

clift::FunctionOp
clift::importFunctionDeclaration(mlir::ModuleOp Module,
                                 mlir::Location DebugLocation,
                                 llvm::StringRef Name,
                                 llvm::StringRef Handle,
                                 clift::FunctionType Prototype,
                                 const model::Function::TypeOfAttributes
                                   &Attributes) {
  mlir::OpBuilder Builder(Module.getContext());
  mlir::OpBuilder::InsertionGuard Guard(Builder);
  Builder.setInsertionPointToEnd(Module.getBody());

  auto Result = Builder.create<clift::FunctionOp>(DebugLocation,
                                                  Name,
                                                  Prototype);
  Result.setHandle(Handle);

  mlir::MLIRContext *Context = Module.getContext();
  mlir::ArrayAttr CliftAttributes = mlir::ArrayAttr::get(Context, {});
  for (model::FunctionAttribute::Values Attribute : Attributes) {
    // TODO: we might want to express some of these through existing clift
    //       attributes.
    switch (Attribute) {
    case model::FunctionAttribute::NoReturn:
      CliftAttributes = mlir::clift::setAttribute<"_Noreturn",
                                                  false>(Context,
                                                         CliftAttributes);
      break;

    case model::FunctionAttribute::Inline:
      CliftAttributes = mlir::clift::setAttribute<"inline",
                                                  false>(Context,
                                                         CliftAttributes);
      break;

    default:
      revng_abort("Unsupported model::FunctionAttribute");
    }
  }

  Result->setAttr("clift.attributes", CliftAttributes);

  return Result;
}

clift::GlobalVariableOp
clift::importSegmentDeclaration(mlir::ModuleOp Module,
                                mlir::Location DebugLocation,
                                llvm::StringRef Name,
                                llvm::StringRef Handle,
                                clift::ValueType Type) {
  mlir::OpBuilder Builder(Module.getContext());
  mlir::OpBuilder::InsertionGuard Guard(Builder);
  Builder.setInsertionPointToEnd(Module.getBody());

  auto Result = Builder.create<clift::GlobalVariableOp>(DebugLocation,
                                                        Name,
                                                        Type);
  Result.setHandle(Handle);

  return Result;
}

void clift::importAllModelTypes(const model::Binary &Model,
                                mlir::ModuleOp Module) {
  mlir::MLIRContext *Context = Module->getContext();
  Context->loadDialect<clift::CliftDialect>();

  mlir::Location Loc = mlir::UnknownLoc::get(Context);
  auto EmitError = [&]() -> mlir::InFlightDiagnostic {
    return Context->getDiagEngine().emit(Loc, mlir::DiagnosticSeverity::Error);
  };

  llvm::SmallVector<mlir::Attribute> TypeAttrs;
  for (const auto &ModelType : Model.TypeDefinitions()) {
    auto CliftType = clift::importModelType(EmitError, *Context, *ModelType);

    TypeAttrs.push_back(mlir::TypeAttr::get(CliftType));
  }

  Module->setAttr("clift.types", mlir::ArrayAttr::get(Context, TypeAttrs));
}

template<typename FunctionT, typename RankT, typename... ArgsT>
clift::FunctionOp importModelFunctionDeclaration(const FunctionT &MF,
                                                 RankT &Rank,
                                                 mlir::ModuleOp Module,
                                                 const model::Binary &Binary) {
  auto EmitError =
    [Context = Module.getContext()]() -> mlir::InFlightDiagnostic {
    return Context->getDiagEngine().emit(mlir::UnknownLoc::get(Context),
                                         mlir::DiagnosticSeverity::Error);
  };

  auto ModelPrototype = Binary.prototypeOrDefault(MF.prototype());
  revng_check(ModelPrototype);

  auto CliftType = mlir::clift::importModelType(EmitError,
                                                *Module.getContext(),
                                                *ModelPrototype);
  auto Prototype = mlir::cast<mlir::clift::FunctionType>(CliftType);

  // NOTE: neither debug information nor name matter for the users of this.
  std::string Handle = pipeline::locationString(Rank, MF.key());
  auto UnknownLocation = mlir::UnknownLoc::get(Module.getContext());
  return mlir::clift::importFunctionDeclaration(Module,
                                                UnknownLocation,
                                                toString(MF.key()),
                                                Handle,
                                                Prototype,
                                                MF.Attributes());
}

void clift::importAllModelFunctionDeclarations(const model::Binary &Model,
                                               mlir::ModuleOp Module) {
  for (const auto &ModelFunction : Model.Functions()) {
    importModelFunctionDeclaration(ModelFunction,
                                   revng::ranks::Function,
                                   Module,
                                   Model);
  }

  for (const auto &ModelFunction : Model.ImportedDynamicFunctions()) {
    importModelFunctionDeclaration(ModelFunction,
                                   revng::ranks::DynamicFunction,
                                   Module,
                                   Model);
  }
}

void clift::importAllModelSegmentDeclarations(const model::Binary &Model,
                                              mlir::ModuleOp Module) {
  for (const auto &Segment : Model.Segments()) {
    auto EmitError =
      [Context = Module.getContext()]() -> mlir::InFlightDiagnostic {
      return Context->getDiagEngine().emit(mlir::UnknownLoc::get(Context),
                                           mlir::DiagnosticSeverity::Error);
    };

    mlir::clift::ValueType SegmentType;
    if (const model::StructDefinition *SegmentStruct = Segment.type()) {
      SegmentType = mlir::clift::importModelType(EmitError,
                                                 *Module.getContext(),
                                                 *SegmentStruct);

    } else {
      static constexpr auto Unsigned = mlir::clift::PrimitiveKind::UnsignedKind;
      auto Char = mlir::clift::PrimitiveType::get(Module.getContext(),
                                                  Unsigned,
                                                  1);
      SegmentType = mlir::clift::ArrayType::get(Char, Segment.VirtualSize());
    }

    // NOTE: neither debug information nor name matter for the users of this.
    std::string Handle = pipeline::locationString(revng::ranks::Segment,
                                                  Segment.key());
    auto UnknownLocation = mlir::UnknownLoc::get(Module.getContext());
    mlir::clift::importSegmentDeclaration(Module,
                                          UnknownLocation,
                                          toString(Segment.key()),
                                          Handle,
                                          SegmentType);
  }
}

namespace rr = revng::ranks;

namespace {

// Helper class for mutating the attribute dictionary of a function parameter.
// All attributes associated with a given function parameter are stored in a
// dictionary attribute, which is by its nature immutable. Changing individual
// function parameter attributes is difficult and inefficient. This class allows
// changes to all function parameter attribute dictionaries to be aggregated and
// applied all at once.
class ArgumentAttributeMutator {
  clift::FunctionOp Function;
  llvm::SmallVector<mlir::NamedAttrList> AttrLists;

public:
  explicit ArgumentAttributeMutator(clift::FunctionOp Op) : Function(Op) {
    for (unsigned I = 0; I < Op.getArgCount(); ++I) {
      AttrLists.emplace_back(Op.getArgAttrs(I));
    }
  }

  mlir::Attribute get(unsigned Index, llvm::StringRef Name) const {
    return AttrLists[Index].get(Name);
  }

  void set(unsigned Index, llvm::StringRef Name, mlir::Attribute Attr) {
    AttrLists[Index].set(Name, Attr);
  }

  void setString(unsigned Index, llvm::StringRef Name, llvm::StringRef Value) {
    set(Index, Name, mlir::StringAttr::get(Function.getContext(), Value));
  }

  void commit() {
    llvm::SmallVector<mlir::Attribute> ArgAttrs;
    for (const mlir::NamedAttrList &AttrList : AttrLists)
      ArgAttrs.push_back(AttrList.getDictionary(Function.getContext()));

    Function.setArgAttrsAttr(mlir::ArrayAttr::get(Function.getContext(),
                                                  ArgAttrs));
  }
};

// Helper class used for recording symbol renames and applying them all at once.
class SymbolRenamer {
  llvm::DenseMap<llvm::StringRef, std::string> Map;

public:
  void record(clift::GlobalOpInterface Op, llvm::StringRef NewName) {
    auto [Iterator, Inserted] = Map.try_emplace(Op.getName(), NewName.str());
    revng_assert(Inserted);
  }

  void apply(mlir::ModuleOp Module) {
    Module->walk([this](mlir::Operation *Op) {
      if (auto Global = mlir::dyn_cast<clift::GlobalOpInterface>(Op)) {
        if (auto It = Map.find(Global.getName()); It != Map.end()) {
          Global.setName(It->second);
        }
      } else if (auto Use = mlir::dyn_cast<clift::UseOp>(Op)) {
        if (auto It = Map.find(Use.getSymbolName()); It != Map.end()) {
          Use.setSymbolName(It->second);
        }
      }
    });

    Map.clear();
  }
};

// Visitor used for applying names to operations, types and their members found
// by visiting a given operation and all nested operations. Any module-level
// operations are not renamed directly, but instead the renames are recorded
// in the specified SymbolRenamer, to be applied all at once.
class NameImporter : public clift::ModuleVisitor<NameImporter> {
  struct CurrentFunctionState {
    using LocationType = pipeline::Location<decltype(rr::Function)>;

    LocationType Location;
    model::CNameBuilder::VariableNameBuilder Variables;
    model::CNameBuilder::GotoLabelNameBuilder GotoLabels;

    explicit CurrentFunctionState(NameImporter &Importer,
                                  LocationType &&Location,
                                  const model::Function &Function) :
      Location(std::move(Location)),
      Variables(Importer.NameBuilder.localVariables(Function)),
      GotoLabels(Importer.NameBuilder.gotoLabels(Function)) {}
  };

  const model::Binary &Model;
  SymbolRenamer &Symbols;
  model::CNameBuilder NameBuilder;

  std::optional<CurrentFunctionState> CurrentFunction;

public:
  explicit NameImporter(const model::Binary &Model, SymbolRenamer &Symbols) :
    Model(Model), Symbols(Symbols), NameBuilder(Model) {}

  //===---------------------- ModuleVisitor interface ---------------------===//

  mlir::LogicalResult visitType(mlir::Type Type) {
    if (auto T = mlir::dyn_cast<clift::FunctionType>(Type)) {
      if (auto L = pipeline::locationFromString(rr::HelperFunction,
                                                T.getHandle())) {
        T.getMutableName().setValue(sanitizeIdentifier(L->back()));

      } else {
        const model::TypeDefinition *MT = getModelType(T.getHandle(),
                                                       rr::TypeDefinition);
        revng_assert(MT != nullptr);

        T.getMutableName().setValue(sanitizeIdentifier(NameBuilder.name(*MT)));
        T.getMutableComment().setValue(MT->Comment());
        if (auto *CFT = llvm::dyn_cast<model::CABIFunctionDefinition>(MT))
          T.getMutableReturnValueComment().setValue(CFT->ReturnValueComment());
        else if (auto *RFT = llvm::dyn_cast<model::CABIFunctionDefinition>(MT))
          T.getMutableReturnValueComment().setValue(RFT->ReturnValueComment());
      }
    }

    return mlir::success();
  }

  mlir::LogicalResult visitAttr(mlir::Attribute Attr) {
    auto T = mlir::dyn_cast<clift::TypeDefinitionAttr>(Attr);
    if (not T)
      return mlir::success();

    if (const auto *MT = getModelType(T.getHandle(), rr::TypeDefinition))
      return visitTypeDefinition(T, *MT);

    if (const auto *MT = getModelType(T.getHandle(), rr::ArtificialStruct)) {
      const auto *FMT = llvm::cast<model::RawFunctionDefinition>(MT);
      return visitArtificialStruct(mlir::cast<clift::StructAttr>(T), *FMT);
    }

    if (const auto *MT = getModelType(T.getHandle(), rr::RawStackArguments)) {
      const auto *FMT = llvm::cast<model::RawFunctionDefinition>(MT);
      return visitRawStackArguments(mlir::cast<clift::StructAttr>(T), *FMT);
    }

    if (auto L = pipeline::locationFromString(rr::HelperStructType,
                                              T.getHandle())) {
      return visitHelperStructType(mlir::cast<clift::StructAttr>(Attr), *L);
    }

    revng_abort("Unsupported type location");
  }

  mlir::LogicalResult visitNestedOp(mlir::Operation *Op) {
    if (auto S = mlir::dyn_cast<clift::MakeLabelOp>(Op))
      return visitMakeLabelOp(S);

    if (auto S = mlir::dyn_cast<clift::LocalVariableOp>(Op))
      return visitLocalVariableOp(S);

    return mlir::success();
  }

  mlir::LogicalResult visitModuleLevelOp(mlir::Operation *Op) {
    if (auto F = mlir::dyn_cast<clift::FunctionOp>(Op))
      return visitFunctionOp(F);

    if (auto G = mlir::dyn_cast<clift::GlobalVariableOp>(Op))
      return visitGlobalVariableOp(G);

    return mlir::success();
  }

private:
  //===----------------------------- Utilities ----------------------------===//

  template<typename RankT, typename ObjectT>
  struct LocationObjectPair {
    pipeline::Location<RankT> Location;
    const ObjectT &Object;
  };

  template<typename RankT, typename ContainerT>
  std::optional<LocationObjectPair<RankT, typename ContainerT::value_type>>
  getModelObject(llvm::StringRef Handle,
                 const RankT &Rank,
                 const ContainerT &Container) {
    using PairType = LocationObjectPair<RankT, typename ContainerT::value_type>;

    if (auto L = pipeline::locationFromString(Rank, Handle)) {
      const auto &[Key] = L->back();
      auto It = Container.find(Key);
      if (It != Container.end())
        return std::optional<PairType>(std::in_place, *L, *It);
    }
    return std::nullopt;
  }

  template<typename RankT>
  const model::TypeDefinition *
  getModelType(llvm::StringRef Handle, const RankT &Rank) {
    if (auto L = pipeline::locationFromString(Rank, Handle)) {
      auto It = Model.TypeDefinitions().find(L->back());
      if (It != Model.TypeDefinitions().end())
        return It->get();
    }
    return nullptr;
  }

  template<typename TypeDefinitionT = model::TypeDefinition>
  const TypeDefinitionT *getModelType(const model::Type &Type) {
    if (const auto *D = llvm::dyn_cast<model::DefinedType>(&Type))
      return llvm::dyn_cast<TypeDefinitionT>(D->Definition().get());
    return nullptr;
  }

  //===------------------------- Type name import -------------------------===//

  mlir::LogicalResult importStructNames(clift::StructAttr ST,
                                        const model::StructDefinition &SMT) {
    for (auto F : ST.getFields()) {
      const auto &Field = SMT.Fields().at(F.getOffset());
      F.getMutableName().setValue(NameBuilder.name(SMT, Field));
      F.getMutableComment().setValue(SMT.Comment());
    }

    return mlir::success();
  }

  mlir::LogicalResult visitTypeDefinition(clift::TypeDefinitionAttr T,
                                          const model::TypeDefinition &MT) {
    T.getMutableName().setValue(NameBuilder.name(MT));
    T.getMutableComment().setValue(MT.Comment());

    if (auto ST = mlir::dyn_cast<clift::StructAttr>(T))
      return importStructNames(ST, llvm::cast<model::StructDefinition>(MT));

    if (auto UT = mlir::dyn_cast<clift::UnionAttr>(T)) {
      const auto &UMT = llvm::cast<model::UnionDefinition>(MT);

      for (auto [I, F] : llvm::enumerate(UT.getFields())) {
        const auto &Field = UMT.Fields().at(static_cast<uint64_t>(I));
        F.getMutableName().setValue(NameBuilder.name(UMT, Field));
        F.getMutableComment().setValue(Field.Comment());
      }

      return mlir::success();
    }

    if (auto ET = mlir::dyn_cast<clift::EnumAttr>(T)) {
      const auto &EMT = llvm::cast<model::EnumDefinition>(MT);

      for (auto E : ET.getFields()) {
        const auto &Entry = EMT.Entries().at(E.getRawValue());
        E.getMutableName().setValue(NameBuilder.name(EMT, Entry));
        E.getMutableComment().setValue(Entry.Comment());
      }

      return mlir::success();
    }

    if (auto TT = mlir::dyn_cast<clift::TypedefAttr>(T))
      return mlir::success();

    revng_abort("Unsupported type definition attribute.");
  }

  mlir::LogicalResult
  visitArtificialStruct(clift::StructAttr ST,
                        const model::RawFunctionDefinition &FMT) {
    revng_assert(ST.getFields().size() == FMT.ReturnValues().size());

    std::string
      Name = (Model.Configuration().Naming().ArtificialReturnValuePrefix()
              + NameBuilder.name(FMT));

    ST.getMutableName().setValue(Name);
    ST.getMutableComment().setValue(FMT.ReturnValueComment());

    for (auto [F, R] : llvm::zip(ST.getFields(), FMT.ReturnValues())) {
      F.getMutableName().setValue(NameBuilder.name(FMT, R));
      F.getMutableComment().setValue(R.Comment());
    }

    return mlir::success();
  }

  mlir::LogicalResult
  visitRawStackArguments(clift::StructAttr ST,
                         const model::RawFunctionDefinition &FMT) {
    const auto &SAT = *FMT.StackArgumentsType();
    const auto *SMT = getModelType<model::StructDefinition>(SAT);
    revng_assert(SMT != nullptr);

    ST.getMutableName().setValue(NameBuilder.name(FMT));
    ST.getMutableComment().setValue(SMT->Comment());

    return importStructNames(ST, *SMT);
  }

  mlir::LogicalResult
  visitHelperStructType(clift::StructAttr ST,
                        const pipeline::Location<decltype(rr::HelperStructType)>
                          &L) {
    std::string
      Name = (Model.Configuration().Naming().ArtificialReturnValuePrefix()
              + sanitizeIdentifier(L.back()));

    ST.getMutableName().setValue(Name);

    for (auto [I, F] : llvm::enumerate(ST.getFields())) {
      std::string Name;
      {
        llvm::raw_string_ostream Out(Name);
        Out << "field_" << I;
      }
      F.getMutableName().setValue(Name);
    }

    return mlir::success();
  }

  //===----------------------- Operation name import ----------------------===//

  auto getModelFunction(llvm::StringRef Handle) {
    return getModelObject(Handle, rr::Function, Model.Functions());
  }

  auto getModelDynamicFunction(llvm::StringRef Handle) {
    return getModelObject(Handle,
                          rr::DynamicFunction,
                          Model.ImportedDynamicFunctions());
  }

  const model::Segment *getModelSegment(clift::GlobalVariableOp Op) {
    auto L = pipeline::locationFromString(rr::Segment, Op.getHandle());
    if (not L)
      return nullptr;

    auto Key = L->at(rr::Segment);
    auto It = Model.Segments().find(Key);
    if (It == Model.Segments().end())
      return nullptr;

    return &*It;
  }

  static void setStringAttr(mlir::Operation *Op,
                            llvm::StringRef Name,
                            llvm::StringRef Value) {
    Op->setAttr(Name, mlir::StringAttr::get(Op->getContext(), Value));
  }

  SortedVector<MetaAddress> getUserAddressSet(mlir::Value Value) {
    auto GetMetaAddress = [](mlir::Operation *Op) {
      if (auto Loc = mlir::dyn_cast_or_null<mlir::NameLoc>(Op->getLoc())) {
        if (auto L = pipeline::locationFromString(rr::Instruction,
                                                  Loc.getName().str())) {
          revng_assert(L->back().isValid());
          return L->back();
        }
      }
      return MetaAddress::invalid();
    };

    SortedVector<MetaAddress> AddressSet;
    for (const auto &User : Value.getUsers()) {
      MetaAddress Address = GetMetaAddress(User);

      if (not Address.isValid()) {
        AddressSet.clear();
        break;
      }

      AddressSet.emplace(Address);
    }
    return AddressSet;
  }

  mlir::LogicalResult visitMakeLabelOp(clift::MakeLabelOp Op) {
    auto AddressSet = getUserAddressSet(Op);

    auto R = CurrentFunction->GotoLabels.name(AddressSet);
    auto L = CurrentFunction->Location.extend(rr::GotoLabel, R.Index);

    // TODO: label comments.
    setStringAttr(Op, "clift.handle", L.toString());
    setStringAttr(Op, "clift.name", sanitizeIdentifier(R.Name));

    return mlir::success();
  }

  mlir::LogicalResult visitLocalVariableOp(clift::LocalVariableOp Op) {
    auto AddressSet = getUserAddressSet(Op);

    auto R = CurrentFunction->Variables.name(AddressSet);
    auto L = CurrentFunction->Location.extend(rr::LocalVariable, R.Index);

    // TODO: variable comments.
    setStringAttr(Op, "clift.handle", L.toString());
    setStringAttr(Op, "clift.name", sanitizeIdentifier(R.Name));

    return mlir::success();
  }

  mlir::LogicalResult visitFunctionOp(clift::FunctionOp Op) {
    CurrentFunction.reset();

    auto ProcessFunction = [this, &Op]<typename MFType>(auto L, MFType &&MF) {
      const auto *Type = Model.prototypeOrDefault(MF.prototype());

      if constexpr (std::same_as<std::decay_t<MFType>, model::Function>)
        CurrentFunction.emplace(*this, std::move(L), MF);

      Symbols.record(Op, NameBuilder.name(MF));

      ArgumentAttributeMutator Attrs(Op);

      using CF = model::CABIFunctionDefinition;
      using RF = model::RawFunctionDefinition;

      if (const auto *T = llvm::dyn_cast<CF>(Type)) {
        revng_assert(Op.getArgCount() == T->Arguments().size());
        auto TL = pipeline::location(rr::TypeDefinition, T->key());

        for (auto [I, A] : llvm::enumerate(T->Arguments())) {
          auto AL = TL.extend(rr::CABIArgument, static_cast<uint64_t>(I));
          Attrs.setString(I, "clift.handle", AL.toString());
          Attrs.setString(I, "clift.name", NameBuilder.name(*T, A));
          Attrs.setString(I, "clift.comment", A.Comment());
        }
      } else if (const auto *T = llvm::dyn_cast<RF>(Type)) {
        bool HasStackArgument = static_cast<bool>(T->StackArgumentsType());

        size_t ArgumentCount = T->Arguments().size() + HasStackArgument;
        revng_assert(Op.getArgCount() == ArgumentCount);

        auto TL = pipeline::location(rr::TypeDefinition, T->key());
        for (auto [I, A] : llvm::enumerate(T->Arguments())) {
          auto AL = TL.extend(rr::RawArgument, A.Location());
          Attrs.setString(I, "clift.handle", AL.toString());
          Attrs.setString(I, "clift.name", NameBuilder.name(*T, A));
          Attrs.setString(I, "clift.comment", A.Comment());

          mlir::ArrayAttr CurrentAttributes;
          if (auto Attributes = Attrs.get(I, "clift.attributes"))
            CurrentAttributes = mlir::cast<mlir::ArrayAttr>(Attributes);
          auto New = mlir::clift::setAttribute<"_REG">(Op.getContext(),
                                                       toString(A.Location()),
                                                       CurrentAttributes);
          Attrs.set(I, "clift.attributes", New);
        }

        if (HasStackArgument) {
          unsigned I = T->Arguments().size();

          auto AL = TL.transmute(rr::RawStackArguments);
          auto Name = Model.Configuration().Naming().RawStackArgumentName();

          Attrs.setString(I, "clift.handle", AL.toString());
          Attrs.setString(I, "clift.name", Name);

          mlir::ArrayAttr CurrentAttributes;
          if (auto Attributes = Attrs.get(I, "clift.attributes"))
            CurrentAttributes = mlir::cast<mlir::ArrayAttr>(Attributes);
          auto New = mlir::clift::setAttribute<"_STACK">(Op.getContext(),
                                                         CurrentAttributes);
          Attrs.set(I, "clift.attributes", New);
        }
      } else {
        revng_abort("Invalid function prototype");
      }

      Attrs.commit();
    };

    if (auto Pair = getModelFunction(Op.getHandle())) {
      ProcessFunction(Pair->Location, Pair->Object);
      return mlir::success();
    }

    if (auto Pair = getModelDynamicFunction(Op.getHandle())) {
      ProcessFunction(Pair->Location, Pair->Object);
      return mlir::success();
    }

    if (auto L = pipeline::locationFromString(rr::HelperFunction,
                                              Op.getHandle())) {
      Symbols.record(Op, sanitizeIdentifier(L->back()));
      return mlir::success();
    }

    revng_abort("Invalid function handle");
  }

  mlir::LogicalResult visitGlobalVariableOp(clift::GlobalVariableOp Op) {
    if (const model::Segment *Segment = getModelSegment(Op)) {
      Symbols.record(Op, NameBuilder.name(Model, *Segment));
      return mlir::success();
    }

    revng_abort("Invalid global variable handle");
  }
};

} // namespace

void mlir::clift::importNames(const model::Binary &Model,
                              mlir::ModuleOp Module) {
  SymbolRenamer Symbols;

  auto R = NameImporter::visit(Module, Model, Symbols);
  revng_assert(R.succeeded());

  Symbols.apply(Module);
}

void mlir::clift::importNames(const model::Function &Function,
                              const model::Binary &Model,
                              mlir::ModuleOp Module) {
  std::unordered_map<MetaAddress, clift::FunctionOp> Functions;
  clift::FunctionOp CliftFunction = nullptr;
  Module->walk([&Function, &CliftFunction](clift::FunctionOp F) {
    MetaAddress MA = getMetaAddress(F);
    if (Function.Entry() == MA) {
      revng_check(CliftFunction == nullptr);
      CliftFunction = F;
    }
  });
  revng_check(CliftFunction != nullptr, "Requested Clift function not found");

  SymbolRenamer Symbols;

  auto R = NameImporter::visit(CliftFunction, Model, Symbols);
  revng_assert(R.succeeded());

  for (mlir::Operation &Op : Module.getBody()->getOperations()) {
    if (auto F = mlir::dyn_cast<clift::FunctionOp>(Op)) {
      if (getMetaAddress(F).isInvalid()) {
        auto R = NameImporter::visit(F, Model, Symbols);
        revng_assert(R.succeeded());
      }
    } else if (auto G = mlir::dyn_cast<clift::GlobalVariableOp>(Op)) {
      auto R = NameImporter::visit(G, Model, Symbols);
      revng_assert(R.succeeded());
    }
  }

  Symbols.apply(Module);
}

static const mlir::DialectRegistry &getDialectRegistry() {
  static const mlir::DialectRegistry Registry = []() -> mlir::DialectRegistry {
    mlir::DialectRegistry Registry;
    Registry.insert<mlir::clift::CliftDialect>();
    return Registry;
  }();
  return Registry;
}

static std::unique_ptr<mlir::MLIRContext> makeContext() {
  const auto Threading = mlir::MLIRContext::Threading::DISABLED;
  return std::make_unique<mlir::MLIRContext>(getDialectRegistry(), Threading);
}

std::pair<mlir::ModuleOp, std::unique_ptr<mlir::MLIRContext>>
mlir::clift::makeHeaderModule(const model::Binary &Model) {
  std::pair<mlir::ModuleOp, std::unique_ptr<mlir::MLIRContext>> Result;
  Result.second = makeContext();

  Result.first = ModuleOp::create(mlir::UnknownLoc::get(Result.second.get()));

  clift::setModuleAttr(Result.first);

  importAllModelTypes(Model, Result.first);
  importAllModelFunctionDeclarations(Model, Result.first);
  importAllModelSegmentDeclarations(Model, Result.first);

  importNames(Model, Result.first);

  return Result;
}
