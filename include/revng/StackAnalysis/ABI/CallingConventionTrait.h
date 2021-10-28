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

  /// States whether double-GPR-sized objects (4-8 bytes on 32-bit systems and
  /// 8-16 bytes on 64-bit systems) are only allowed to start from a register
  /// with an even index. For example, the ABI for a function like
  /// ```
  /// void function(uint32_t, uint64_t);
  /// ```
  /// on a system with 32-bit wide general purpose registers would lead to
  /// the first argument being placed in the first argument register (say `r0`),
  /// but then, because the second register is double-GRP-sized, its location
  /// depends on this parameter. If `OnlyStartDoubleArgumentsFromAnEvenRegister`
  /// is true, the argument is passed using a pair of registers starting from
  /// the next one with an even index (`r2` and `r3` in this example),
  /// otherwise, the next pair of registers is used (`r1` and `r2` here).
  ///
  /// \note: the option has no effect if `ArgumentsArePositionBased` is true.
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;

  /// States whether specified ABI allows splitting a single big (with size
  /// exceeding the size of a single register) between the register and the
  /// stack if there's not enough free registers to fit it
  /// For example, let's say there's a big object of type `A` such that
  /// `sizeof(A) == 16` bytes (on 32-bit architecture, that would mean that four
  /// general purpose registers (`16 == 4 * 4`) are required to fit it).
  /// And let's assume that the ABI in question has four GPRs available for
  /// argument passing (`r0-r3`). Then, the ABI for a function like
  /// ```
  /// void function(uint32_t, A);
  /// ```
  /// would allocate the first argument into the `r0` register, but then
  /// there would not be enough space to pass the second argument.
  ///
  /// In that case, if `ArgumentsCanBeSplitBetweenRegistersAndStack` is true,
  /// the first 12 bytes of the second argument would still be placed into
  /// `r1-r3` and the last 4 bytes would go on the stack.
  /// Otherwise, the entire object would go on the stack with the remaining
  /// three registers unused.
  ///
  /// \note: the option has no effect if `ArgumentsArePositionBased` is true.
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;

  /// Specifies whether vector registers are always used in rigid pairs, meaning
  /// that an odd-index vector register cannot be used without the even-index
  /// register from the same pair being used to hold half the value.
  ///
  /// \note: The option still is not fully implemented since the vector register
  /// support is still super limited.
  static constexpr bool VectorRegistersArePaired = true;

  /// States whether specified ABI allows accepting aggregate object arguments
  /// (structs, unions, etc) in the designated registers if they provide enough
  /// space (including padding) and the maximum number of registers allowed for
  /// each of the arguments.
  ///
  /// `0` means that using registers for ANY aggregate registers is disallowed,
  /// even the object could fit into a single general purpose register.
  ///
  /// `1` means that passing values is only allowed if their size (including
  /// padding) doesn't exceed the size of a single general purpose register.
  ///
  /// `2` means that passing aggregate values is only allowed if their size
  /// (including padding) doesn't exceed the size of two general purpose
  /// registers.
  ///
  /// ... and so on.
  ///
  /// if `MaximumGeneralPurposeRegistersPerArgument` is greater than or
  /// equal to `GeneralPurposeArgumentRegisters.size()`, all the general purpose
  /// registers allowed to be used for function arguments could be used for
  /// a single aggregate argument.
  ///
  /// If the argument that doen't fit into the allowed register count it is
  /// placed on the stack.
  ///
  /// \note: some special cases still apply. For example C++ abi doesn't allow
  /// structs and classes that are not 'trivial for the purposes of calls' to
  /// be passed in such a fashion. As such, object with irregular padding,
  /// non-trivial construction/destruction, etc are only passed using the stack.
  /// (see https://itanium-cxx-abi.github.io/cxx-abi)
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;

  /// States whether specified ABI allows returning aggregate objects (structs
  /// unions, etc) in the designated registers if they provide enough space
  /// space (including padding) and the maximum number of registers allowed for
  /// those values.
  ///
  /// The rules from `MaxGeneralPurposeRegistersPerAggregateArgument` apply,
  /// see relevant comment for additional information.
  /// \see MaxGeneralPurposeRegistersPerAggregateArgument
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;

  /// States whether specified ABI allows accepting primite object arguments
  /// (`int`s, `long`s, `__int128`s, etc) in the designated registers if they
  /// provide enough space (including padding) and the maximum number of
  /// registers allowed per argument.
  ///
  /// `0` means that using general purpose registers is not allowed.
  ///
  /// `1` means that passing values is only allowed if their size doesn't
  /// exceed the size of a single general purpose register.
  ///
  /// `2` means that passing aggregate values is only allowed if their size
  /// doesn't exceed the size of two general purpose registers.
  ///
  /// ... and so on.
  ///
  /// if `MaxGeneralPurposeRegistersPerPrimitiveArgument` is greater than or
  /// equal to `GeneralPurposeArgumentRegisters.size()`, all the general purpose
  /// registers allowed to be used for function arguments could be used for
  /// a single aggregate argument.
  ///
  /// If the argument that doesn't fit into the allowed register count it is
  /// placed on the stack.
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 0;

  /// States whether specified ABI allows returning primitive objects (`int`s,
  /// `long`s, `__int128`s, etc) in the designated registers if they provide
  /// enough space and the maximum number of registers allowed per argument.
  ///
  /// The rules from `MaxGeneralPurposeRegistersPerPrimitiveArgument` apply,
  /// see relevant comment for additional information.
  /// \see MaxGeneralPurposeRegistersPerPrimitiveArgument
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 0;

  /// States who is responsible for cleaning the stack after the function call.
  /// `true` - callee, `false` - caller.
  static constexpr bool CalleeIsResponsibleForStackCleanup = false;

  /// States the alignment of the stack in bytes.
  ///
  /// \note: states minimum value for abis supporting multiple different stack
  /// alignment values, for example, if the ABI requires the stack to be aligned
  /// on 4 bytes for internal calls but on 8 bytes for interfaces, the value of
  /// `StackAlignment` should be equal to 4.
  static constexpr size_t StackAlignment = 1;

  /// Stores the list of general purpose registers allowed to be used for
  /// passing arguments and the order they are to be used in.
  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};

  /// Stores the list of general purpose registers allowed to be used for
  /// returning values and the order they are to be used in.
  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeReturnValueRegisters = {};

  /// Stores the list of vector registers allowed to be used for passing
  /// arguments and the order they are to be used in.
  static constexpr std::array<model::Register::Values, 0>
    VectorArgumentRegisters = {};

  /// Stores the list of vector registers allowed to be used for returning
  /// values and the order they are to be used in.
  static constexpr std::array<model::Register::Values, 0>
    VectorReturnValueRegisters = {};

  /// Stores the list of registers value of which has to be preserved by
  /// the callee, meaning when the callee returns, the value of those registers
  /// must be the same as it was when the function was called.
  static constexpr std::array<model::Register::Values, 0>
    CalleeSavedRegisters = {};

  /// Specifies a register to be used for returning (or even passing,
  /// depending on ABI) the pointer to the memory used for returning
  /// copies of big aggregate objects.
  ///
  /// Can be `model::Register::Invalid` for ABIs that do not support returning
  /// values by 'pointer-to-copy'.
  static constexpr auto PointerToCopyRegister = model::Register::Invalid;
};

/// System V x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_64> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 8;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 2;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 8;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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
    model::Register::r15_x86_64, model::Register::fs_x86_64
  };

  static constexpr auto PointerToCopyRegister = model::Register::rdi_x86_64;
};

/// The default Microsoft x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x64> {
  static constexpr bool ArgumentsArePositionBased = true;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 1;

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

  static constexpr auto PointerToCopyRegister = model::Register::rax_x86_64;
};

/// The `__vectorcall` Microsoft x64 ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x64_vectorcall> {
  static constexpr bool ArgumentsArePositionBased = true;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 1;

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

  static constexpr auto PointerToCopyRegister = model::Register::rax_x86_64;
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
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 0;

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

  static constexpr auto PointerToCopyRegister = model::Register::Invalid;
};

/// The default SystemV x86 ABI
/// The parameters are only passed using the stack.
/// \todo: look into the st-return values.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array<model::Register::Values, 0>
    GeneralPurposeArgumentRegisters = {};
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86,
    model::Register::edx_x86
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86,
    model::Register::xmm1_x86,
    model::Register::xmm2_x86
  };
  static constexpr std::array VectorReturnValueRegisters = {
    // model::Register::st0_x86,
    model::Register::xmm0_x86
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `regparm(1)` SystemV x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_regparm_1> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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
    VectorArgumentRegisters = { model::Register::xmm0_x86,
                                model::Register::xmm1_x86,
                                model::Register::xmm2_x86 };
  static constexpr std::array VectorReturnValueRegisters = {
    // model::Register::st0_x86,
    model::Register::xmm0_x86
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `regparm(2)` SystemV x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_regparm_2> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86,
    model::Register::xmm1_x86,
    model::Register::xmm2_x86
  };
  static constexpr std::array VectorReturnValueRegisters = {
    // model::Register::st0_x86,
    model::Register::xmm0_x86
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `regparm(3)` SystemV x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::SystemV_x86_regparm_3> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86,
    model::Register::xmm1_x86,
    model::Register::xmm2_x86
  };
  static constexpr std::array VectorReturnValueRegisters = {
    // model::Register::st0_x86,
    model::Register::xmm0_x86
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86,
    model::Register::ebp_x86,
    model::Register::esp_x86,
    model::Register::edi_x86,
    model::Register::esi_x86
  };

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `__cdecl` Microsoft x86 ABI
/// The parameters are only passed using the stack.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_cdecl> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `__stdcall` Microsoft x86 ABI
/// The parameters are only passed using the stack.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_stdcall> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `__fastcall` Microsoft x86 ABI
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_fastcall> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// `__thiscall` Microsoft x86 ABI
/// Function arguments are passed using both stack and the specified register.
/// The register always contains the first parameter - `this` pointer.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_thiscall> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

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

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
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
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 0;

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

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// The `__vectorcall` Microsoft x86 ABI.
/// Function arguments are passed using both stack and specified registers.
///
/// \note: this ABI is equivalent to `__stdcall` except it also allows passing
/// arguments in AVX vector registers.
template<>
struct CallingConventionTrait<model::abi::Microsoft_x86_vectorcall> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = false;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = false;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 0;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 1;

  static constexpr bool CalleeIsResponsibleForStackCleanup = true;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::ecx_x86,
    model::Register::edx_x86
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::eax_x86
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::xmm0_x86, model::Register::xmm1_x86,
    model::Register::xmm2_x86, model::Register::xmm3_x86,
    model::Register::xmm4_x86, model::Register::xmm5_x86
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::xmm0_x86
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::ebx_x86, model::Register::ebp_x86,
    model::Register::esp_x86, model::Register::edi_x86,
    model::Register::esi_x86, model::Register::xmm6_x86,
    model::Register::xmm7_x86
  };

  static constexpr auto PointerToCopyRegister = model::Register::eax_x86;
};

/// The Aarch64 (ARM) ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Aarch64> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = true;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = true;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 2;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 2;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 2;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 16;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::x0_aarch64, model::Register::x1_aarch64,
    model::Register::x2_aarch64, model::Register::x3_aarch64,
    model::Register::x4_aarch64, model::Register::x5_aarch64,
    model::Register::x6_aarch64, model::Register::x7_aarch64
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::x0_aarch64, model::Register::x1_aarch64,
    model::Register::x2_aarch64, model::Register::x3_aarch64,
    model::Register::x4_aarch64, model::Register::x5_aarch64,
    model::Register::x6_aarch64, model::Register::x7_aarch64
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::v0_aarch64, model::Register::v1_aarch64,
    model::Register::v2_aarch64, model::Register::v3_aarch64,
    model::Register::v4_aarch64, model::Register::v5_aarch64,
    model::Register::v6_aarch64, model::Register::v7_aarch64
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::v0_aarch64, model::Register::v1_aarch64,
    model::Register::v2_aarch64, model::Register::v3_aarch64,
    model::Register::v4_aarch64, model::Register::v5_aarch64,
    model::Register::v6_aarch64, model::Register::v7_aarch64
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::x19_aarch64, model::Register::x20_aarch64,
    model::Register::x21_aarch64, model::Register::x22_aarch64,
    model::Register::x23_aarch64, model::Register::x24_aarch64,
    model::Register::x25_aarch64, model::Register::x26_aarch64,
    model::Register::x27_aarch64, model::Register::x28_aarch64,
    model::Register::x29_aarch64, model::Register::sp_aarch64,
    model::Register::v8_aarch64,  model::Register::v9_aarch64,
    model::Register::v10_aarch64, model::Register::v11_aarch64,
    model::Register::v12_aarch64, model::Register::v13_aarch64,
    model::Register::v14_aarch64, model::Register::v15_aarch64
  };

  static constexpr auto PointerToCopyRegister = model::Register::x8_aarch64;
};

/// The Aarch32 (ARM) ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::Aarch32> {
  static constexpr bool ArgumentsArePositionBased = false;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = true;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = true;
  static constexpr bool VectorRegistersArePaired = false;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 4;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 1;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 4;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 4;

  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 4;
  // \note: the stack must be double-aligned (8 bytes) in public interfaces.

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::r0_arm,
    model::Register::r1_arm,
    model::Register::r2_arm,
    model::Register::r3_arm
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::r0_arm,
    model::Register::r1_arm,
    model::Register::r2_arm,
    model::Register::r3_arm
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::q0_arm,
    model::Register::q1_arm,
    model::Register::q2_arm,
    model::Register::q3_arm
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::q0_arm,
    model::Register::q1_arm,
    model::Register::q2_arm,
    model::Register::q3_arm
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::r4_arm,  model::Register::r5_arm,
    model::Register::r6_arm,  model::Register::r7_arm,
    model::Register::r8_arm,  model::Register::r10_arm,
    model::Register::r11_arm, model::Register::r13_arm,
    model::Register::q4_arm,  model::Register::q5_arm,
    model::Register::q6_arm,  model::Register::q7_arm
  };

  static constexpr auto PointerToCopyRegister = model::Register::r0_arm;
};

/// The "old" 32-bit MIPS ABI.
/// Function arguments are passed using both stack and specified registers.
template<>
struct CallingConventionTrait<model::abi::MIPS_o32> {
  static constexpr bool ArgumentsArePositionBased = true;
  static constexpr bool OnlyStartDoubleArgumentsFromAnEvenRegister = true;
  static constexpr bool ArgumentsCanBeSplitBetweenRegistersAndStack = true;
  static constexpr bool VectorRegistersArePaired = true;

  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateArgument = 4;
  static constexpr size_t MaxGeneralPurposeRegistersPerAggregateReturnValue = 2;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveArgument = 4;
  static constexpr size_t MaxGeneralPurposeRegistersPerPrimitiveReturnValue = 2;

  static constexpr bool CalleeIsResponsibleForStackCleanup = false;
  static constexpr size_t StackAlignment = 4;

  static constexpr std::array GeneralPurposeArgumentRegisters = {
    model::Register::a0_mips,
    model::Register::a1_mips,
    model::Register::a2_mips,
    model::Register::a3_mips
  };
  static constexpr std::array GeneralPurposeReturnValueRegisters = {
    model::Register::v0_mips,
    model::Register::v1_mips
  };

  static constexpr std::array VectorArgumentRegisters = {
    model::Register::f12_mips,
    model::Register::f13_mips,
    model::Register::f14_mips,
    model::Register::f15_mips
  };
  static constexpr std::array VectorReturnValueRegisters = {
    model::Register::f0_mips,
    model::Register::f1_mips,
    model::Register::f2_mips,
    model::Register::f3_mips
  };

  static constexpr std::array CalleeSavedRegisters = {
    model::Register::s0_mips,  model::Register::s1_mips,
    model::Register::s2_mips,  model::Register::s3_mips,
    model::Register::s4_mips,  model::Register::s5_mips,
    model::Register::s6_mips,  model::Register::s7_mips,
    model::Register::s8_mips,  model::Register::gp_mips,
    model::Register::sp_mips,  model::Register::f20_mips,
    model::Register::f21_mips, model::Register::f22_mips,
    model::Register::f23_mips, model::Register::f24_mips,
    model::Register::f25_mips, model::Register::f26_mips,
    model::Register::f27_mips, model::Register::f28_mips,
    model::Register::f29_mips, model::Register::f30_mips,
    model::Register::f31_mips
  };

  static constexpr auto PointerToCopyRegister = model::Register::v0_mips;
};

} // namespace abi
