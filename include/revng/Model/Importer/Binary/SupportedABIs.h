#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <array>
#include <span>

#include "revng/Model/ABI.h"

inline std::span<const model::ABI::Values>
supportedABIListForELF(model::Architecture::Values V) {
  switch (V) {
  case model::Architecture::x86_64: {
    static constexpr std::array Options{ model::ABI::SystemV_x86_64 };
    return Options;
  }
  case model::Architecture::x86: {
    static constexpr std::array Options{ model::ABI::SystemV_x86,
                                         model::ABI::SystemV_x86_regparm_1,
                                         model::ABI::SystemV_x86_regparm_2,
                                         model::ABI::SystemV_x86_regparm_3 };
    return Options;
  }
  case model::Architecture::aarch64: {
    static constexpr std::array Options{ model::ABI::AAPCS64 };
    return Options;
  }
  case model::Architecture::arm: {
    static constexpr std::array Options{ model::ABI::AAPCS };
    return Options;
  }
  case model::Architecture::mips: {
    static constexpr std::array Options{ model::ABI::SystemV_MIPS_o32 };
    return Options;
  }
  case model::Architecture::mipsel: {
    static constexpr std::array Options{ model::ABI::SystemV_MIPSEL_o32 };
    return Options;
  }
  case model::Architecture::systemz: {
    static constexpr std::array Options{ model::ABI::SystemZ_s390x };
    return Options;
  }

  case model::Architecture::Invalid:
  case model::Architecture::Count:
  default:
    revng_abort();
  }
}

inline std::span<const model::ABI::Values>
supportedABIListForPECOFF(model::Architecture::Values V) {
  switch (V) {
  case model::Architecture::x86_64: {
    static constexpr std::array Options{
      model::ABI::Microsoft_x86_64,
      model::ABI::Microsoft_x86_64_vectorcall,
      model::ABI::Microsoft_x86_64_clrcall
    };
    return Options;
  }
  case model::Architecture::x86: {
    static constexpr std::array Options{
      model::ABI::Microsoft_x86_cdecl,      model::ABI::Microsoft_x86_stdcall,
      model::ABI::Microsoft_x86_fastcall,   model::ABI::Microsoft_x86_thiscall,
      model::ABI::Microsoft_x86_vectorcall, model::ABI::Microsoft_x86_clrcall
    };
    return Options;
  }
  case model::Architecture::aarch64: {
    static constexpr std::array Options{ model::ABI::AAPCS64 };
    return Options;
  }
  case model::Architecture::arm: {
    static constexpr std::array Options{ model::ABI::AAPCS };
    return Options;
  }
  case model::Architecture::mips: {
    static constexpr std::array Options{ model::ABI::SystemV_MIPS_o32 };
    return Options;
  }
  case model::Architecture::mipsel: {
    static constexpr std::array Options{ model::ABI::SystemV_MIPSEL_o32 };
    return Options;
  }
  case model::Architecture::systemz: {
    static constexpr std::array Options{ model::ABI::SystemZ_s390x };
    return Options;
  }
  default:
    revng_abort();
  }
}
