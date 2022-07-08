#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <utility>

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

#include "revng/ADT/StringTemplateParameter.h"
#include "revng/Pipeline/Registry.h"

namespace pipeline {

struct ArtifactLocationKindBase;

/// Instantiate a global object of this class for each new location kind you
/// wish to register
class RegisterArtifactLocation : Registry {
private:
  ArtifactLocationKindBase *K;

public:
  RegisterArtifactLocation(ArtifactLocationKindBase &K) : K(&K) {}
  ~RegisterArtifactLocation() override = default;

public:
  void registerLocations(ArtifactLocationRegistry &Dictionary) override;

  void registerContainersAndPipes(Loader &) override {}
  void registerKinds(KindsRegistry &KindDictionary) override {}
  void libraryInitialization() override {}
};

} // namespace pipeline
