#pragma once

#include "mlir/IR/Attributes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LLVM.h"

#include "revng/Clift/CliftAttrInterfaces.h"
#include "revng/Pipeline/Location.h"
#include "revng/Pipes/Ranks.h"

namespace mlir::clift {

inline std::optional<mlir::clift::DefinedType>
lookupCliftType(mlir::ModuleOp &Module,
                const model::TypeDefinition &ModelType) {
  auto LookingFor = pipeline::locationString(revng::ranks::TypeDefinition,
                                             ModelType.key());

  // TODO: Is there a better way than just iterating over all the types?
  auto TypeList = Module->getAttrOfType<mlir::ArrayAttr>("clift.type-list");
  revng_check(TypeList != nullptr);
  revng_check(not TypeList.empty());

  for (mlir::Attribute Attribute : TypeList)
    if (mlir::TypeAttr Attr = mlir::dyn_cast<mlir::TypeAttr>(Attribute))
      if (auto Dfnd = mlir::dyn_cast<mlir::clift::DefinedType>(Attr.getValue()))
        if (Dfnd.getHandle() == LookingFor)
          return Dfnd;

  return std::nullopt;
}

} // namespace mlir::clift
