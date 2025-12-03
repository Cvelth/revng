#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "mlir/IR/Attributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LLVM.h"

#include "revng/Clift/CliftAttrInterfaces.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"

namespace mlir::clift {

inline mlir::clift::DefinedType
lookupCliftType(mlir::ModuleOp Module, const model::TypeDefinition &ModelType) {
  auto LookingFor = pipeline::locationString(revng::ranks::TypeDefinition,
                                             ModelType.key());

  // TODO: Is there a better way than just iterating over all the types?
  auto TypeList = Module->getAttrOfType<mlir::ArrayAttr>("clift.types");
  revng_check(TypeList != nullptr);
  revng_check(not TypeList.empty());

  for (mlir::Attribute Attribute : TypeList)
    if (mlir::TypeAttr Attr = mlir::dyn_cast<mlir::TypeAttr>(Attribute))
      if (auto D = mlir::dyn_cast<mlir::clift::DefinedType>(Attr.getValue()))
        if (D.getHandle() == LookingFor)
          return D;

  return nullptr;
}

} // namespace mlir::clift
