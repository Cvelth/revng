// \file RegisterKind.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipeline/ArtifactLocationRegistry.h"
#include "revng/Pipeline/RegisterArtifactLocation.h"

namespace pipeline {

using RegisterAL = RegisterArtifactLocation;
void RegisterAL::registerLocations(ArtifactLocationRegistry &Dictionary) {
  Dictionary.registerLocation(*K);
}

} // namespace pipeline
