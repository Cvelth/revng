#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/Support/CommandLine.h"

#include "revng/Support/CommandLine.h"

inline llvm::cl::list<std::string> ImportDebugInfo("import-debug-info",
                                                   llvm::cl::desc("path"),
                                                   llvm::cl::ZeroOrMore,
                                                   llvm::cl::cat(MainCategory));

#define DESCRIPTION                                                      \
  "Fetch debug info from canonical places or web. The dependency level " \
  "should be greater than 1 in order to process the dependency libraries."
using OptLevel = llvm::cl::opt<unsigned>;
inline OptLevel EnableRemoteDebugInfoWithLevel("enable-remote-debug-info",
                                               llvm::cl::desc(DESCRIPTION),
                                               llvm::cl::value_desc("dependency"
                                                                    " "
                                                                    "level"),
                                               llvm::cl::cat(MainCategory),
                                               llvm::cl::init(0));
#undef DESCRIPTION

enum DebugInfoAction { no, yes, libraries };

#define DESCRIPTION                                                       \
  "During the import-model phase, this option controls whether we:\n 1) " \
  "ignore debug info\n 2) consider debug info from the file itself\n 3) " \
  "consider debug info from libraries that the program depends on\n"
using DebugInfoOptAction = llvm::cl::opt<DebugInfoAction>;
inline DebugInfoOptAction
  DebugInfoAction("debug-info",
                  llvm::cl::desc(DESCRIPTION),
                  llvm::cl::value_desc("debug-"
                                       "info "
                                       "action"),
                  llvm::cl::values(clEnumVal(no, "Ignore Debug Info"),
                                   clEnumVal(yes, "Consider Debug Info"),
                                   clEnumVal(libraries,
                                             "Consider Debug Info in "
                                             "Depending "
                                             "Libraries")),
                  llvm::cl::cat(MainCategory),
                  llvm::cl::init(yes));
#undef DESCRIPTION

struct DebugInfoOptions {
  bool IgnoreDebugInfo = false;

  unsigned FetchDebugInfoWithLevel = 0;
};

namespace {
inline void parseDebugInfoOptions(DebugInfoOptions &TheDebugInfoOptions) {
  if (DebugInfoAction == DebugInfoAction::no) {
    TheDebugInfoOptions.IgnoreDebugInfo = true;
  } else if (DebugInfoAction == DebugInfoAction::yes) {
    TheDebugInfoOptions.FetchDebugInfoWithLevel = 1;
  } else if (DebugInfoAction == DebugInfoAction::libraries) {
    TheDebugInfoOptions.FetchDebugInfoWithLevel = 2;
  }

  // If EnableRemoteDebugInfoWithLevel option was set, override the fetching
  // level.
  if (EnableRemoteDebugInfoWithLevel != 0)
    TheDebugInfoOptions
      .FetchDebugInfoWithLevel = EnableRemoteDebugInfoWithLevel;
}
} // namespace

#define DESCRIPTION "base address where dynamic objects should be loaded"
inline llvm::cl::opt<uint64_t> BaseAddress("base",
                                           llvm::cl::desc(DESCRIPTION),
                                           llvm::cl::value_desc("address"),
                                           llvm::cl::cat(MainCategory),
                                           llvm::cl::init(0x400000));
#undef DESCRIPTION
