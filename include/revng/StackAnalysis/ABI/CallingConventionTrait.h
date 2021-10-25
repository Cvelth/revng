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

  /// States whether specified ABI allows using both general purpose and vector
  /// registers in parallel, e.g. whether `GeneralPurposeArgumentRegisters[i]`
  /// can be used for an argument in a functions that also uses
  /// `VectorArgumentRegisters[i]` for a different argument when `i` satisfies
  /// `i < GeneralPurposeArgumentRegisters.size()` and
  /// `i < VectorArgumentRegisters.size()`.
  ///
  /// For an example of an ABI with position based registers see Microsoft x64
  /// documentation, for one without - see SystemV x64 documentation.
  static constexpr bool ArgumentsArePositionBased = false;

  /// States whether aggregate objects (structs, unions, etc) are allowed to
  /// be passed around in registers.
  ///
  /// \note: When `AllowPassingAggregatesInRegisters` is false, but either
  /// `MaximumGeneralPurposeRegistersPerArgument` or
  /// `MaximumGeneralPurposeRegistersPerReturnValue` is non-zero, their
  /// values are used for determining the maximum number of registers to use
  /// for primitive values that are bigger than a general purpose registers but
  /// don't use vector registers (for example `__int128`).
  static constexpr bool AllowPassingAggregatesInRegisters = false;

  /// States whether specified ABI allows accepting aggregate object arguments
  /// (structs, unions, etc) in the designated registers if they provide enough
  /// space (including padding) and the maximum number of registers allowed per
  /// aggregate argument.
  ///
  /// `0` means that using registers for big (aggregate or otherwise) objects
  /// is disallowed.
  ///
  /// `1` means that passing values are only allowed if their size (including
  /// padding) doesn't exceed the size of a single general purpose register.
  ///
  /// `2` means that passing aggregate values are only allowed if their size
  /// (including padding) doesn't exceed the size of two general purpose
  /// registers.
  ///
  /// ...
  ///
  /// if `MaximumGeneralPurposeRegistersPerArgument` is greater than or
  /// equal to `GeneralPurposeArgumentRegisters.size()`, all the general purpose
  /// registers allowed to be used for function arguments could be used for
  /// a single aggregate argument.
  ///
  /// All the non-conforming aggregate values are passed using the stack.
  ///
  /// \note: some special cases still apply. For example C++ abi doesn't allow
  /// structs and classes that are not 'trivial for the purposes of calls' to
  /// be passed in such a fashion.
  /// (see https://itanium-cxx-abi.github.io/cxx-abi)
  ///
  /// \note: unaligned aggregate objects are always passed using stack.
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 0;

  /// States whether specified ABI allows returning aggregate objects (structs,
  /// unions, etc) in the designated registers if they provide enough space
  /// (including padding) and the maximum number of registers allowed per
  /// aggregate argument.
  ///
  /// The rules from `MaximumGeneralPurposeRegistersPerArgument` apply,
  /// see relevant comment for additional information.
  /// \see MaximumGeneralPurposeRegistersPerArgument
  ///
  /// \note The value must either be `0` if using registers for aggregate
  /// return values is not allowed, `1` if it's allowed for a single register
  /// only or equal to the size of `GeneralPurposeReturnValueRegisters`
  /// container.
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 0;

  /// States who is responsible for cleaning the stack after the function call.
  /// `true` - callee, `false` - caller.
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  /// States the alignment of the stack in bytes.
  ///
  /// \note: states minimum value for abis supporting multiple different stack
  /// alignment values.
  static constexpr size_t StackAlignment = 1;

  /// Stores the list of general purpose registers allowed to be used for
  /// argument passing in the order they are used.
  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};

  /// Stores the list of general purpose registers allowed to be used for
  /// return values in order they are used.
  ///
  /// \note: if `MaximumGeneralPurposeRegistersPerReturnValue` is not
  /// equal to either `0` or `1`, the size of this list must not exceed its
  /// value.
  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeReturnValueRegisters = {};

  /// Stores the list of vector registers allowed to be used for argument
  /// passing in the order they are used.
  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};

  /// Stores the list of vector registers allowed to be used for return
  /// values in order they are used.
  /// \note: if `AggregateArgumentsCanOccupySubsequentRegisters` is `false`,
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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = true;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 8;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::rdi_x86_64, model::Register::rsi_x86_64,
    model::Register::rdx_x86_64, model::Register::rcx_x86_64,
    model::Register::r8_x86_64,  model::Register::r9_x86_64
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
    model::Register::rsp_x86_64, model::Register::r12_x86_64,
    model::Register::r13_x86_64, model::Register::r14_x86_64,
    model::Register::r15_x86_64
    // model::Register::fs_x86_64
  };
};

/// The default Microsoft x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x64> {
  static constexpr bool ArgumentsArePositionBased = true;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 1;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::rcx_x86_64,
    model::Register::rdx_x86_64,
    model::Register::r8_x86_64,
    model::Register::r9_x86_64
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
  static constexpr bool ArgumentsArePositionBased = true;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 1;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::rcx_x86_64,
    model::Register::rdx_x86_64,
    model::Register::r8_x86_64,
    model::Register::r9_x86_64
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 1;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeReturnValueRegisters = {};

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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 0;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {
      // model::Register::xmm0_x86,
      // model::Register::xmm1_x86,
      // model::Register::xmm2_x86
    };
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {
      // model::Register::st0_x86,
      // model::Register::xmm0_x86,
    };

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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::eax_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {
      // model::Register::xmm0_x86,
      // model::Register::xmm1_x86,
      // model::Register::xmm2_x86
    };
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {
      // model::Register::st0_x86,
      // model::Register::xmm0_x86,
    };

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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {
      // model::Register::xmm0_x86,
      // model::Register::xmm1_x86,
      // model::Register::xmm2_x86
    };
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {
      // model::Register::st0_x86,
      // model::Register::xmm0_x86,
    };

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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86,
    model::Register::ecx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {
      // model::Register::xmm0_x86,
      // model::Register::xmm1_x86,
      // model::Register::xmm2_x86
    };
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {
      // model::Register::st0_x86,
      // model::Register::xmm0_x86,
    };

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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 0;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 0;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::ecx_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 2;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::ecx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 0;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 0;
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};
  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeReturnValueRegisters = {};

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
///
/// \note: this ABI is equivalent to `__stdcall` except it also allows passing
/// arguments in AVX vector registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_vectorcall> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool AllowPassingAggregatesInRegisters = false;
  static constexpr size_t MaximumGeneralPurposeRegistersPerArgument = 1;
  static constexpr size_t MaximumGeneralPurposeRegistersPerReturnValue = 1;
  static constexpr bool CalleeIsResponsibleForStackCleanup = true;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::ecx_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
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
