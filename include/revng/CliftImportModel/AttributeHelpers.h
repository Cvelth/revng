#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/SmallVector.h"

#include "mlir/IR/BuiltinAttributes.h"

#include "revng/ADT/ConstexprString.h"
#include "revng/Clift/CliftAttributes.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"
#include "revng/Support/Annotations.h"

namespace mlir::clift {

template<ConstexprString Macro, bool IsOurs = true>
mlir::ArrayAttr setAttribute(mlir::MLIRContext *Context,
                             mlir::ArrayAttr AttributeArray = nullptr) {
  if constexpr (IsOurs)
    ptml::Attributes.assertAttributeName<Macro>();

  auto MacroLocation = pipeline::location(revng::ranks::Macro,
                                          llvm::StringRef(Macro).str());

  llvm::SmallVector<mlir::Attribute> ExistingAttributes;
  if (AttributeArray != nullptr) {
    for (mlir::Attribute RawAttribute : AttributeArray) {
      auto Attr = mlir::cast<mlir::clift::AttributeAttr>(RawAttribute);
      if (Attr.getMacro().getString() == Macro) {
        revng_assert(Attr.getMacro().getHandle() == MacroLocation.toString());

        // Already set, nothing to do.
        return AttributeArray;
      }

      ExistingAttributes.emplace_back(Attr);
    }
  }

  using CompAttr = mlir::clift::AttributeComponentAttr;
  auto MacroAttribute = CompAttr::get(Context, Macro, MacroLocation.toString());
  auto FullAttribute = mlir::clift::AttributeAttr::get(Context,
                                                       MacroAttribute,
                                                       std::nullopt);
  ExistingAttributes.emplace_back(FullAttribute);

  return mlir::ArrayAttr::get(Context, ExistingAttributes);
}

// Note, only single-argument version is provided because there are currently
// no multi-argument attributes.

template<ConstexprString Macro, bool IsOurs = true>
mlir::ArrayAttr setAttribute(mlir::MLIRContext *Context,
                             llvm::StringRef Argument,
                             mlir::ArrayAttr AttributeArray = nullptr) {
  if constexpr (IsOurs)
    ptml::Attributes.assertAnnotationName<Macro>();

  auto MacroLocation = pipeline::location(revng::ranks::Macro,
                                          llvm::StringRef(Macro).str());
  auto ArgumentLocation = MacroLocation.extend(revng::ranks::MacroArgument,
                                               Argument);

  bool AlreadyPresent = false;
  llvm::SmallVector<mlir::Attribute> ExistingAttributes;
  if (AttributeArray != nullptr) {
    for (mlir::Attribute RawAttribute : AttributeArray) {
      auto Attr = mlir::cast<mlir::clift::AttributeAttr>(RawAttribute);
      if (Attr.getMacro().getString() == Macro) {
        revng_assert(Attr.getMacro().getHandle() == MacroLocation.toString());
        revng_assert(not AlreadyPresent, "The same attribute twice");

        // Already present, update the argument.
        using CompAttr = mlir::clift::AttributeComponentAttr;
        auto ArgumentAttribute = CompAttr::get(Context,
                                               Argument,
                                               ArgumentLocation.toString());
        auto FullAttribute = mlir::clift::AttributeAttr::get(Context,
                                                             Attr.getMacro(),
                                                             ArgumentAttribute);
        ExistingAttributes.emplace_back(FullAttribute);
        AlreadyPresent = true;
      } else {
        ExistingAttributes.emplace_back(Attr);
      }
    }
  }

  if (not AlreadyPresent) {
    using CompAttr = mlir::clift::AttributeComponentAttr;
    auto MacroAttribute = CompAttr::get(Context,
                                        Macro,
                                        MacroLocation.toString());
    auto ArgumentAttribute = CompAttr::get(Context,
                                           Argument,
                                           ArgumentLocation.toString());
    auto FullAttribute = mlir::clift::AttributeAttr::get(Context,
                                                         MacroAttribute,
                                                         { ArgumentAttribute });
    ExistingAttributes.emplace_back(FullAttribute);
  }

  return mlir::ArrayAttr::get(Context, ExistingAttributes);
}

} // namespace mlir::clift
