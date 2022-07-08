// \file Registry.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipeline/ArtifactLocationRegistry.h"
#include "revng/Pipeline/KindsRegistry.h"
#include "revng/Pipeline/Registry.h"

namespace pipeline {

KindsRegistry Registry::registerAllKinds() {
  KindsRegistry Registry;
  for (const auto &Reg : getInstances())
    Reg->registerKinds(Registry);

  Kind::init();
  Rank::init();
  return Registry;
}

ArtifactLocationRegistry Registry::registerAllLocations() {
  ArtifactLocationRegistry Registry;
  for (const auto &Reg : getInstances())
    Reg->registerLocations(Registry);

  return Registry;
}

} // namespace pipeline
