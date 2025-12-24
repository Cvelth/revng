//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/CliftEmitC/CCommentEmitter.h"
#include "revng/PTML/CTokenEmitter.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"

using CCE = mlir::clift::CCommentEmitter;
void CCE::emitFunctionComment(mlir::clift::FunctionOp Function) {
  auto Guard = Tokens.enterRegion(ptml::CTokenEmitter::RegionKind::Commentable,
                                  Function.getHandle());

  // Function comment
  dbg << Function.getFunctionType().getComment().str();
  emitDoxygenComment(Function.getFunctionType().getComment());

  // `\param` comments
  bool NewlineEmitted = false;
  for (unsigned I = 0; I < Function.getArgCount(); ++I) {
    auto Attrs = Function.getArgAttrs(I);
    auto GetStringAttr = [&Attrs](llvm::StringRef Name) {
      if (auto Attribute = Attrs.get(Name))
        return mlir::cast<mlir::StringAttr>(Attribute).getValue();
      else
        return llvm::StringRef{};
    };

    llvm::StringRef CommentBody = GetStringAttr("clift.comment");
    if (CommentBody.empty())
      continue;

    if (not NewlineEmitted) {
      emitDoxygenComment(" ");
      NewlineEmitted = true;
    }

    llvm::StringRef Handle = GetStringAttr("clift.handle");
    revng_assert(not Handle.empty());
    auto ArgG = Tokens.enterRegion(ptml::CTokenEmitter::RegionKind::Commentable,
                                   Handle);

    llvm::StringRef Name = GetStringAttr("clift.name");
    CommentEmitter::Identifier Identifier{
      .Name = Name,
      .CustomEmitter =
        [Name, Handle, this]() {
          Tokens
            .emitIdentifier(Name,
                            Handle,
                            ptml::CTokenEmitter::EntityKind::FunctionParameter,
                            ptml::CTokenEmitter::IdentifierKind::Reference);
        },
    };

    static constexpr llvm::StringRef DoxygenArgumentKeyword = "param";
    emitDoxygenLineImpl(DoxygenArgumentKeyword, CommentBody, Identifier);
  }

  // `\returns` comment
  auto RVCommentBody = Function.getFunctionType().getReturnValueComment();
  if (not RVCommentBody.empty()) {
    emitDoxygenComment(" ");

    auto FunctionTypeHandle = Function.getFunctionType().getHandle();
    auto FTLoc = pipeline::locationFromString(revng::ranks::TypeDefinition,
                                              FunctionTypeHandle);
    revng_assert(FTLoc.has_value());
    auto RVLoc = FTLoc->transmute(revng::ranks::ReturnValue).toString();

    using RegionKind = ptml::CTokenEmitter::RegionKind;
    auto Guard = Tokens.enterRegion(RegionKind::Commentable, RVLoc);

    static constexpr llvm::StringRef DoxygenArgumentKeyword = "returns";
    emitDoxygenLineImpl(DoxygenArgumentKeyword, RVCommentBody);
  }
}
