#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/Model/Architecture.h"
#include "revng/Model/TupleTree.h"
#include "revng/Support/Debug.h"
#include "revng/Support/YAMLTraits.h"

namespace model::abi {

enum Values {
  Invalid,

  SystemV_x86_64,
  SystemV_x86,
  SystemV_x86_regparm_3,
  SystemV_x86_regparm_2,
  SystemV_x86_regparm_1,
  // SystemV_x86_sseregparm_3,
  // SystemV_x86_sseregparm_2,
  // SystemV_x86_sseregparm_1,

  Microsoft_x64,
  Microsoft_x64_vectorcall,
  Microsoft_x64_clrcall,

  Microsoft_x86_cdecl,
  Microsoft_x86_stdcall,
  Microsoft_x86_thiscall,
  Microsoft_x86_fastcall,
  Microsoft_x86_clrcall,
  Microsoft_x86_vectorcall,

  Count
};

inline constexpr llvm::StringRef getName(Values V) {
  switch (V) {
  case Invalid:
    return "Invalid";

  case SystemV_x86_64:
    return "SystemV_x86_64";
  case SystemV_x86:
    return "SystemV_x86";
  case SystemV_x86_regparm_3:
    return "SystemV_x86_regparm_3";
  case SystemV_x86_regparm_2:
    return "SystemV_x86_regparm_2";
  case SystemV_x86_regparm_1:
    return "SystemV_x86_regparm_1";

  case Microsoft_x64:
    return "Microsoft_x64";
  case Microsoft_x64_vectorcall:
    return "Microsoft_x64_vectorcall";
  case Microsoft_x64_clrcall:
    return "Microsoft_x64_clrcall";

  case Microsoft_x86_cdecl:
    return "Microsoft_x86_cdecl";
  case Microsoft_x86_stdcall:
    return "Microsoft_x86_stdcall";
  case Microsoft_x86_thiscall:
    return "Microsoft_x86_thiscall";
  case Microsoft_x86_fastcall:
    return "Microsoft_x86_fastcall";
  case Microsoft_x86_clrcall:
    return "Microsoft_x86_clrcall";
  case Microsoft_x86_vectorcall:
    return "Microsoft_x86_vectorcall";

  default:
    revng_abort();
  }
}

inline constexpr model::Architecture::Values getArchitecture(Values V) {
  switch (V) {
  case SystemV_x86_64:
  case Microsoft_x64:
  case Microsoft_x64_clrcall:
  case Microsoft_x64_vectorcall:
    return model::Architecture::x86_64;

  case SystemV_x86:
  case SystemV_x86_regparm_3:
  case SystemV_x86_regparm_2:
  case SystemV_x86_regparm_1:
  case Microsoft_x86_clrcall:
  case Microsoft_x86_vectorcall:
  case Microsoft_x86_cdecl:
  case Microsoft_x86_stdcall:
  case Microsoft_x86_fastcall:
  case Microsoft_x86_thiscall:
    return model::Architecture::x86;

  case Count:
  case Invalid:
  default:
    revng_abort();
  }
}

} // namespace model::abi

namespace llvm::yaml {

template<>
struct ScalarEnumerationTraits<model::abi::Values>
  : public NamedEnumScalarTraits<model::abi::Values> {};

} // namespace llvm::yaml
