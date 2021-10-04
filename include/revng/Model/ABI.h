#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/StringRef.h"

#include "revng/Model/TupleTree.h"
#include "revng/Support/Debug.h"
#include "revng/Support/YAMLTraits.h"

namespace model::abi {

enum Values {
  Invalid,

  SystemV_x86_64,

  Microsoft_x64,
  Microsoft_cdecl, // x86 only
  Microsoft_clrcall,
  Microsoft_stdcall, // x86 only
  Microsoft_fastcall, // x86 only
  Microsoft_thiscall, // x86 only
  Microsoft_vectorcall,
  Microsoft_regparm_3,
  Microsoft_regparm_2,
  Microsoft_regparm_1,

  Count
};

inline llvm::StringRef getName(Values V) {
  switch (V) {
  case Invalid:
    return "Invalid";
  case SystemV_x86_64:
    return "SystemV_x86_64";
  case Microsoft_x64:
    return "Microsoft_x64";
  case Microsoft_cdecl:
    return "Microsoft_cdecl";
  case Microsoft_clrcall:
    return "Microsoft_clrcall";
  case Microsoft_stdcall:
    return "Microsoft_stdcall";
  case Microsoft_fastcall:
    return "Microsoft_fastcall";
  case Microsoft_thiscall:
    return "Microsoft_thiscall";
  case Microsoft_vectorcall:
    return "Microsoft_vectorcall";
  case Microsoft_regparm_3:
    return "Microsoft_regparm_3";
  case Microsoft_regparm_2:
    return "Microsoft_regparm_2";
  case Microsoft_regparm_1:
    return "Microsoft_regparm_1";
  default:
    revng_abort();
  }
  revng_abort();
}

} // namespace model::abi

namespace llvm::yaml {

template<>
struct ScalarEnumerationTraits<model::abi::Values>
  : public NamedEnumScalarTraits<model::abi::Values> {};

} // namespace llvm::yaml
