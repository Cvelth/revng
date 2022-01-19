#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"

int verifyABI(const TupleTree<model::Binary> &Binary,
              llvm::StringRef RuntimeArtifact,
              model::ABI::Values ABI,
              llvm::raw_fd_ostream &OutputStream);
