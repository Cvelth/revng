#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"

int convertToCABI(TupleTree<model::Binary> &Binary,
                  model::abi::Values ABI,
                  llvm::raw_fd_ostream &OutputStream);
int convertToRaw(TupleTree<model::Binary> &Binary,
                 model::abi::Values ABI,
                 llvm::raw_fd_ostream &OutputStream);
