#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/Support/Error.h"

#define check_error_container(error_container) \
  if (error_container == nullptr) {            \
    ErrorContainer tmp_ec;                     \
    error_container = &tmp_ec;                 \
  } else {                                     \
    revng_check(error_container->empty());     \
  }

class ErrorContainer {
private:
  std::vector<llvm::Error> Errors;

public:
  ErrorContainer() : Errors(){};

  ~ErrorContainer() {
    for (auto &Error : Errors) {
      llvm::consumeError(std::move(Error));
    }
  }

  bool empty() const { return Errors.empty(); }

  void add(llvm::Error &&Error) {
    if (Error)
      Errors.push_back(std::move(Error));
    else
      llvm::consumeError(std::move(Error));
  }

  size_t count() const { return Errors.size(); }

  llvm::Error *get(size_t Index) { return &Errors[Index]; }

  template<typename T>
  T fail(llvm::Error &&Error, T ReturnValue) {
    add(std::move(Error));
    return ReturnValue;
  }

  bool failIf(llvm::Error &&Error) {
    if (Error) {
      add(std::move(Error));
      return false;
    }
    return true;
  }

  template<typename T>
  T failIf(llvm::Error &&Error, T SuccessValue, T FailValue) {
    if (Error) {
      add(std::move(Error));
      return FailValue;
    }
    return SuccessValue;
  }
};
