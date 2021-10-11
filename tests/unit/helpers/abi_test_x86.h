#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

namespace ABI_TEST {

constexpr const char *Input_x86 = R"(---
Architecture:    x86
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
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000001
    Arguments: []
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000002
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000003
    Arguments:
      - Location:        ebx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000004
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000005
    Arguments:
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000006
    Arguments:
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000007
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000008
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        ebx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000009
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        ebx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000010
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000011
    Arguments: []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000012
    Arguments: []
    ReturnValues:
      - Location:        ebx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000013
    Arguments: []
    ReturnValues:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000014
    Arguments: []
    ReturnValues:
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000015
    Arguments: []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000016
    Arguments: []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000017
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000018
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000019
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_IDs{ 1000000001, 1000000011 };
constexpr const char *SystemV_x86 = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             SystemV_x86
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             SystemV_x86
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_regparm_1_IDs{ 1000000001,
                                                             1000000002,
                                                             1000000011,
                                                             1000000019 };
constexpr const char *SystemV_x86_regparm_1 = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             SystemV_x86_regparm_1
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000002
    ABI:             SystemV_x86_regparm_1
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             SystemV_x86_regparm_1
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000019
    ABI:             SystemV_x86_regparm_1
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_regparm_2_IDs{
  1000000001, 1000000002, 1000000006, 1000000007, 1000000011, 1000000019
};
constexpr const char *SystemV_x86_regparm_2 = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000002
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000006
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000007
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000019
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_regparm_3_IDs{
  1000000001, 1000000002, 1000000006, 1000000007,
  1000000010, 1000000011, 1000000019
};
constexpr const char *SystemV_x86_regparm_3 = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000002
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000006
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000007
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000010
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000019
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_cdecl_IDs{ 1000000001,
                                                           1000000011,
                                                           1000000015 };
constexpr const char *Microsoft_x86_cdecl = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !Struct
    Kind:            Struct
    ID:              9000000015
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          4
    Size:            8
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x86_cdecl
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             Microsoft_x86_cdecl
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000015
    ABI:             Microsoft_x86_cdecl
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000015"
    Arguments:       []
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_stdcall_IDs{ 1000000001,
                                                             1000000011,
                                                             1000000015 };
constexpr const char *Microsoft_x86_stdcall = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !Struct
    Kind:            Struct
    ID:              9000000015
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          4
    Size:            8
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x86_stdcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             Microsoft_x86_stdcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000015
    ABI:             Microsoft_x86_stdcall
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000015"
    Arguments:       []
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_thiscall_IDs{ 1000000001,
                                                              1000000004,
                                                              1000000011,
                                                              1000000015 };
constexpr const char *Microsoft_x86_thiscall = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !Struct
    Kind:            Struct
    ID:              9000000015
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          4
    Size:            8
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x86_thiscall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000004
    ABI:             Microsoft_x86_thiscall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             Microsoft_x86_thiscall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000015
    ABI:             Microsoft_x86_thiscall
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000015"
    Arguments:       []
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_fastcall_IDs{ 1000000001,
                                                              1000000004,
                                                              1000000011,
                                                              1000000015,
                                                              1000000018 };
constexpr const char *Microsoft_x86_fastcall = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !Struct
    Kind:            Struct
    ID:              9000000015
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-772"
        Offset:          4
    Size:            8
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x86_fastcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000004
    ABI:             Microsoft_x86_fastcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             Microsoft_x86_fastcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000015
    ABI:             Microsoft_x86_fastcall
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000015"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000018
    ABI:             Microsoft_x86_fastcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_clrcall_IDs{ 1000000001 };
constexpr const char *Microsoft_x86_clrcall = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x86_clrcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_vectorcall_IDs{ 1000000001,
                                                                1000000004,
                                                                1000000011,
                                                                1000000018 };
constexpr const char *Microsoft_x86_vectorcall = R"(---
Architecture:    x86
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x86_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000004
    ABI:             Microsoft_x86_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000011
    ABI:             Microsoft_x86_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000018
    ABI:             Microsoft_x86_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-772"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

} // namespace ABI_TEST
