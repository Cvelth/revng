#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Binary.h"
#include "revng/Model/Register.h"
#include "revng/Model/Type.h"

namespace abi {

template<model::abi::Values V>
struct AllowedRegisterList {
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

  static constexpr std::array<model::Register::Values, 0>
    CalleeSavedRegisters = {};
};

template<>
struct AllowedRegisterList<model::abi::SystemV_x86_64> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = false;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = true;

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

template<>
struct AllowedRegisterList<model::abi::Microsoft_x64> {
  static constexpr bool VectorArgumentsReplaceGenericOnes = true;
  static constexpr bool AllowAnArgumentToOccupySubsequentRegisters = false;

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

} // namespace abi
