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
  /// Unknown and/or unsupported ABI.
  Invalid,

  /// 64-bit SystemV ABI for x86 processor architecture.
  /// The latest version of the documentation can be found
  /// [here](https://gitlab.com/x86-psABIs/x86-64-ABI)
  SystemV_x86_64,

  /// 32-bit SystemV ABI for x86 processor architecture.
  /// The latest version of the documentation can be found
  /// [here](https://gitlab.com/x86-psABIs/i386-ABI/-/tree/hjl/x86/master)
  SystemV_x86,

  /// A gcc specific modification of the 32-bit SystemV ABI for x86 processor
  /// architecture. It allows three first GPR-sized arguments to be passed
  /// using the EAX, EDX, and ECX registers.
  /// \see `regparm` x86 function attribute.
  SystemV_x86_regparm_3,

  /// A gcc specific modification of the 32-bit SystemV ABI for x86 processor
  /// architecture. It allows two first GPR-sized arguments to be passed
  /// using the EAX, and ECX registers.
  /// \see `regparm` x86 function attribute.
  SystemV_x86_regparm_2,

  /// A gcc specific modification of the 32-bit SystemV ABI for x86 processor
  /// architecture. It allows the first GPR-sized argument to be passed
  /// using the EAX register.
  /// \see `regparm` x86 function attribute.
  SystemV_x86_regparm_1,

  /// 64-bit Microsoft ABI for x86 processor architecture.
  /// The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention)
  Microsoft_x64,

  /// A modification of 64-bit Microsoft ABI for x86 processor architecture.
  /// It allows using extra vector registers for passing function arguments.
  /// The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/vectorcall)
  Microsoft_x64_vectorcall,

  /// A modification of 64-bit Microsoft ABI for x86 processor architecture.
  /// It uses CLR expression stack to pass function arguments.
  /// The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/clrcall)
  Microsoft_x64_clrcall,

  /// The default 32-bit Microsoft ABI for x86 processor architecture.
  /// It was indented to be compatible with `SystemV_x86` but there are slight
  /// differences. The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/cdecl)
  Microsoft_x86_cdecl,

  /// A modification of the 32-bit `__cdecl` Microsoft ABI for x86 processor
  /// architecture. The main difference is the fact that the callee is
  /// responsible for stack cleanup instead of the caller.
  /// The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/stdcall)
  Microsoft_x86_stdcall,

  /// A modification of the 32-bit `__stdcall` Microsoft ABI for x86 processor
  /// architecture. The main difference is the fact that it allows to pass a
  /// single (the first) function argument using a register. This ABI is only
  /// used for member function call where the first argument is always a `this`
  /// pointer. The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/thiscall)
  Microsoft_x86_thiscall,

  /// A modification of the 32-bit `__stdcall` Microsoft ABI for x86 processor
  /// architecture. The main difference is the fact that it allows to pass two
  /// first GPR-sized non-aggregate function arguments in registers.=
  /// The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/fastcall)
  Microsoft_x86_fastcall,

  /// A modification of 32-bit Microsoft ABI for x86 processor architecture.
  /// It uses CLR expression stack to pass function arguments.
  /// The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/clrcall)
  Microsoft_x86_clrcall,

  /// A modification of the 32-bit `__fastcall` Microsoft ABI for x86 processor
  /// architecture. It allows using extra vector registers for passing function
  /// arguments. The documentation can be found
  /// [here](https://docs.microsoft.com/en-us/cpp/cpp/vectorcall)
  Microsoft_x86_vectorcall,

  /// The ABI for Aarch64 (ARM) processor architecture.
  /// The latest version of the documentation can be found
  /// [here](https://github.com/ARM-software/abi-aa/releases)
  Aarch64,

  /// The ABI for Aarch32 (ARM) processor architecture.
  /// The latest version of the documentation can be found
  /// [here](https://github.com/ARM-software/abi-aa/releases)
  Aarch32,

  /// The ABI for MIPS RISC processor architecture.
  /// The latest version of the documentation can be found
  /// [here](http://math-atlas.sourceforge.net/devel/assembly/mipsabi32.pdf)
  MIPS_o32,

  /// The s390x ABI for SystemZ processor architecture.
  /// The latest version of the documentation can be found
  /// [here](https://github.com/IBM/s390x-abi)
  SystemZ_s390x,

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

  case Aarch64:
    return "Aarch64";
  case Aarch32:
    return "Aarch32";

  case MIPS_o32:
    return "MIPS_o32";

  case SystemZ_s390x:
    return "SystemZ_s390x";

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

  case Aarch64:
    return model::Architecture::aarch64;
  case Aarch32:
    return model::Architecture::arm;

  case MIPS_o32:
    return model::Architecture::mips;

  case SystemZ_s390x:
    return model::Architecture::systemz;

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
