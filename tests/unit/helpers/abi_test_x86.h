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
  - !Primitive
    Kind:            Primitive
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000001
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000002
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000003
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000004
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000005
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000006
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000007
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000008
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000009
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000010
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !Struct
    Kind:            Struct
    ID:              9000000000
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          8
    Size:            16
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000011
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000012
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000013
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000014
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000015
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000016
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000017
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000018
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
  - !Struct
    Kind:            Struct
    ID:              9000000019
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          24
    Size:            32
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000019
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000019"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000020
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000019"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000021
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000019"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000019"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-9000000019"
  - !Struct
    Kind:            Struct
    ID:              9000000022
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          24
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          40
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          48
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          56
    Size:            64
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000022
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000022"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000023
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000022"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000022"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000024
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000022"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000025
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000026
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000022"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_IDs{
  1000000001, 1000000011, 1000000015,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026
};
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
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              9000000000
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          8
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              9000000019
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          24
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              9000000022
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          24
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          40
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          48
      - Type:
          UnqualifiedType: "/Types/Primitive-776"
        Offset:          56
    Size:            64
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000015
    ABI:             SystemV_x86
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000001
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000002
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000003
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000017
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000019
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000020
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000023
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000025
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000026
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 0
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
