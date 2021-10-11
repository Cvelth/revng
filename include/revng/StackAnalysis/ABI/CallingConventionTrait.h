#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/Type.h"

namespace abi {

/// A trait struct which is specialized for all of the supported ABIs.
/// It describes the specifics of the calling convention for the ABI.
template<model::abi::Values V>
struct CallingConventionTrait {

  /// States whether specified ABI supports using both generic and vector
  /// registers in parallel.
  /// For example of `true` calling ABIs see Microsoft x64 one, for `false` -
  /// see SystemV x64 ABI.
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;

  /// States whether specified ABI supports splitting a single parameter among
  /// a couple of subsequent registers.
  /// For example of `true` calling ABIs see SystemV x64 one, for `false` -
  /// see Microsoft x64 ABI.
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;

  /// States who is responsible for cleaning the stack when returning from
  /// a function, if `true` - the Callee is responsible, otherwise, the Caller.
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  /// States the alignment of the stack in bytes.
  static constexpr size_t StackAlignment = 1;

  /// Stores the list of generic registers allowed to be used for argument
  /// passing in the order they are used.
  static constexpr std::array<model::Register::Values, 0>
    GenericArgumentRegisters = {};

  /// Stores the list of generic registers allowed to be used for return
  /// values in order they are used.
  /// \note: if `AllowAnArgumentToOccupySubsequentRegisters` is `false`,
  /// the size of this list must not exceed 1.
  static constexpr std::array<model::Register::Values, 0>
    GenericReturnValueRegisters = {};

  /// Stores the list of vector registers allowed to be used for argument
  /// passing in the order they are used.
  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};

  /// Stores the list of vector registers allowed to be used for return
  /// values in order they are used.
  /// \note: if `AllowAnArgumentToOccupySubsequentRegisters` is `false`,
  /// the size of this list must not exceed 1.
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  /// Stores the list of registers value of which has to be preserved by
  /// the callee, meaning when the callee returns, the value of those registers
  /// must be the same as it was when the function was called.
  static constexpr std::array<model::Register::Values, 0>
    CalleeSavedRegisters = {};
};

/// System V x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_64> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::rdi_x86_64, model::Register::rsi_x86_64,
    model::Register::rdx_x86_64, model::Register::rcx_x86_64,
    model::Register::r8_x86_64,  model::Register::r9_x86_64
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::rax_x86_64,
    model::Register::rdx_x86_64
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86_64, model::Register::xmm1_x86_64,
    model::Register::xmm2_x86_64, model::Register::xmm3_x86_64,
    model::Register::xmm4_x86_64, model::Register::xmm5_x86_64,
    model::Register::xmm6_x86_64, model::Register::xmm7_x86_64
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::xmm0_x86_64,
    model::Register::xmm1_x86_64
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::rbx_x86_64, model::Register::rbp_x86_64,
    model::Register::r12_x86_64, model::Register::r13_x86_64,
    model::Register::r14_x86_64, model::Register::r15_x86_64
  };
};

/// The default Microsoft x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x64> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = true;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::rcx_x86_64,
    model::Register::rdx_x86_64,
    model::Register::r8_x86_64,
    model::Register::r9_x86_64
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::rax_x86_64
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86_64,
    model::Register::xmm1_x86_64,
    model::Register::xmm2_x86_64,
    model::Register::xmm3_x86_64
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::xmm0_x86_64
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::r12_x86_64,  model::Register::r13_x86_64,
    model::Register::r14_x86_64,  model::Register::r15_x86_64,
    model::Register::rdi_x86_64,  model::Register::rsi_x86_64,
    model::Register::rbx_x86_64,  model::Register::rbp_x86_64,
    model::Register::xmm6_x86_64, model::Register::xmm7_x86_64
  };
};

/// The `__vectorcall` Microsoft x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x64_vectorcall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = true;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::rcx_x86_64,
    model::Register::rdx_x86_64,
    model::Register::r8_x86_64,
    model::Register::r9_x86_64
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::rax_x86_64
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86_64, model::Register::xmm1_x86_64,
    model::Register::xmm2_x86_64, model::Register::xmm3_x86_64,
    model::Register::xmm4_x86_64, model::Register::xmm5_x86_64
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::xmm0_x86_64
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::r12_x86_64,  model::Register::r13_x86_64,
    model::Register::r14_x86_64,  model::Register::r15_x86_64,
    model::Register::rdi_x86_64,  model::Register::rsi_x86_64,
    model::Register::rbx_x86_64,  model::Register::rbp_x86_64,
    model::Register::xmm6_x86_64, model::Register::xmm7_x86_64
  };
};

/// The `__clrcall` microsoft ABI (can be used for executables of either x86 or
/// x64 architecture). The parameters are passed using CLR expression stack.
/// No registers are affected on call.
///
/// \note: it's probably a good idea to double-check `CalleeSavedRegisters`,
/// (the array just lists the defaults), but I'm not sure where I could find
/// that information.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x64_clrcall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr std::array<model::Register::Values, 0>
    GenericArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    GenericReturnValueRegisters = {};

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::r12_x86_64,  model::Register::r13_x86_64,
    model::Register::r14_x86_64,  model::Register::r15_x86_64,
    model::Register::rdi_x86_64,  model::Register::rsi_x86_64,
    model::Register::rbx_x86_64,  model::Register::rbp_x86_64,
    model::Register::xmm6_x86_64, model::Register::xmm7_x86_64
  };
};

/// The default SystemV x86 ABI
/// The parameters are only passed using the stack.
/// \todo: look into the st-return values.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array<model::Register::Values, 0>
    GenericArgumentRegisters = {};
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `regparm(1)` SystemV x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_regparm_1> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::eax_x86
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `regparm(2)` SystemV x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_regparm_2> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `regparm(3)` SystemV x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_regparm_3> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86,
    model::Register::ecx_x86
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `__cdecl` Microsoft x86 ABI
/// The parameters are only passed using the stack.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_cdecl> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 4;

  static constexpr std::array<model::Register::Values, 0>
    GenericArgumentRegisters = {};
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `__stdcall` Microsoft x86 ABI
/// The parameters are only passed using the stack.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_stdcall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;

  static constexpr size_t StackAlignment = 4;

  static constexpr std::array<model::Register::Values, 0>
    GenericArgumentRegisters = {};
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `__fastcall` Microsoft x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_fastcall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;

  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::ecx_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// `__thiscall` Microsoft x86 ABI
/// Function arguments are passed using both stack and the specified register.
/// The register always contains the first parameter - `this` pointer.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_thiscall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;

  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::ecx_x86
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// The `__clrcall` microsoft ABI (can be used for executables of either x86 or
/// x64 architecture). The parameters are passed using CLR expression stack.
/// No registers are affected on call.
///
/// \note: it's probably a good idea to double-check `CalleeSavedRegisters`,
/// (the array just lists the defaults), but I'm not sure where I could find
/// that information.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_clrcall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;

  static constexpr std::array<model::Register::Values, 0>
    GenericArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    GenericReturnValueRegisters = {};

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };
};

/// The `__vectorcall` Microsoft x86 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_vectorcall> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = true;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GenericArgumentRegisters = {
    model::Register::ecx_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GenericReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {
      // model::Register::xmm0_x86,
      // model::Register::xmm1_x86,
      // model::Register::xmm2_x86,
      // model::Register::xmm3_x86,
      // model::Register::xmm4_x86,
      // model::Register::xmm5_x86
    };
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {
      // model::Register::xmm0_x86
    };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
    // model::Register::xmm6_x86,
    // model::Register::xmm7_x86
  };
};

} // namespace abi
