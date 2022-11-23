/// \file Predefined.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ABI/Predefined.h"
#include "revng/Model/ABI.h"
#include "revng/Model/Register.h"

#include "AArch64/AAPCS64.h"
#include "ARM/AAPCS.h"
#include "MIPS/SystemV/o32.h"
#include "MIPSEL/SystemV/o32.h"
#include "SystemZ/s390x.h"
#include "x86/Microsoft/cdecl.h"
#include "x86/Microsoft/clrcall.h"
#include "x86/Microsoft/fastcall.h"
#include "x86/Microsoft/stdcall.h"
#include "x86/Microsoft/thiscall.h"
#include "x86/Microsoft/vectorcall.h"
#include "x86/Pascal/register.h"
#include "x86/SystemV/cdecl.h"
#include "x86/SystemV/regparm_1.h"
#include "x86/SystemV/regparm_2.h"
#include "x86/SystemV/regparm_3.h"
#include "x86_64/Microsoft/clrcall.h"
#include "x86_64/Microsoft/default.h"
#include "x86_64/Microsoft/vectorcall.h"
#include "x86_64/SystemV/default.h"

namespace abi::predefined {

const Definition &get(model::ABI::Values ABI) {
  switch (ABI) {
  case model::ABI::SystemV_x86_64:
    return x86_64::SystemV::Default::Defined;
  case model::ABI::SystemV_x86:
    return x86::SystemV::cdecl::Defined;
  case model::ABI::SystemV_x86_regparm_3:
    return x86::SystemV::regparm_3::Defined;
  case model::ABI::SystemV_x86_regparm_2:
    return x86::SystemV::regparm_2::Defined;
  case model::ABI::SystemV_x86_regparm_1:
    return x86::SystemV::regparm_1::Defined;

  case model::ABI::Microsoft_x86_64:
    return x86_64::Microsoft::Default::Defined;
  case model::ABI::Microsoft_x86_64_vectorcall:
    return x86_64::Microsoft::vectorcall::Defined;
  case model::ABI::Microsoft_x86_64_clrcall:
    return x86_64::Microsoft::clrcall::Defined;
  case model::ABI::Microsoft_x86_cdecl:
    return x86::Microsoft::cdecl::Defined;
  case model::ABI::Microsoft_x86_stdcall:
    return x86::Microsoft::stdcall::Defined;
  case model::ABI::Microsoft_x86_thiscall:
    return x86::Microsoft::thiscall::Defined;
  case model::ABI::Microsoft_x86_fastcall:
    return x86::Microsoft::fastcall::Defined;
  case model::ABI::Microsoft_x86_clrcall:
    return x86::Microsoft::clrcall::Defined;
  case model::ABI::Microsoft_x86_vectorcall:
    return x86::Microsoft::vectorcall::Defined;

  case model::ABI::Pascal_x86:
    return x86::Pascal::Register::Defined;

  case model::ABI::AAPCS64:
    return AArch64::AAPCS64::Defined;
  case model::ABI::AAPCS:
    return ARM::AAPCS::Defined;

  case model::ABI::SystemV_MIPS_o32:
    return MIPS::SystemV::o32::Defined;
  case model::ABI::SystemV_MIPSEL_o32:
    return MIPSEL::SystemV::o32::Defined;

  case model::ABI::SystemZ_s390x:
    return SystemZ::s390x::Defined;

  case model::ABI::Count:
  case model::ABI::Invalid:
  default:
    revng_abort();
    break;
  }
}

} // namespace abi::predefined
