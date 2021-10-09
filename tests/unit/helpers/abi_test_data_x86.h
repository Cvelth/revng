#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

namespace ABI_TEST {

constexpr const char *Input_x86 = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
...
)";

constexpr const char *SystemV_x86 = R"(---

)";

constexpr const char *SystemV_x86_regparm_3 = R"(---

)";

constexpr const char *SystemV_x86_regparm_2 = R"(---

)";

constexpr const char *SystemV_x86_regparm_1 = R"(---

)";

constexpr const char *Microsoft_x86_cdecl = R"(---

)";

constexpr const char *Microsoft_x86_stdcall = R"(---

)";

constexpr const char *Microsoft_x86_thiscall = R"(---

)";

constexpr const char *Microsoft_x86_fastcall = R"(---

)";

constexpr const char *Microsoft_x86_clrcall = R"(---

)";

constexpr const char *Microsoft_x86_vectorcall = R"(---

)";

} // namespace ABI_TEST
