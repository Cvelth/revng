#pragma once

#include "revng/ADT/SortedVector.h"

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

namespace ABI_TEST {

constexpr const char *Input_x86_64 = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              513
    PrimitiveKind:   Generic
    Size:            1
  - !Primitive
    Kind:            Primitive
    ID:              514
    PrimitiveKind:   Generic
    Size:            2
  - !Primitive
    Kind:            Primitive
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              520
    PrimitiveKind:   Generic
    Size:            8
  - !Primitive
    Kind:            Primitive
    ID:              528
    PrimitiveKind:   Generic
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000001
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000002
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          1
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000003
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000004
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000004
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          6
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000005
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000006
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000007
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000008
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          12
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          20
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000008
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000009
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000010
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          48
    Size:            64
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
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000003
    Arguments:
      - Location:        rbx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000004
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000005
    Arguments:
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000006
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000007
    Arguments:
      - Location:        rbp_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000008
    Arguments:
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000009
    Arguments:
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000010
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000011
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000012
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000013
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r10_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000014
    Arguments: []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000015
    Arguments: []
    ReturnValues:
      - Location:        rbx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000016
    Arguments: []
    ReturnValues:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000017
    Arguments: []
    ReturnValues:
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000018
    Arguments:
      - Location:        r10_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        r10_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000019
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000020
    Arguments:
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000021
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:    []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000022
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000023
    Arguments: []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000024
    Arguments: []
    ReturnValues:
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000025
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
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
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000003
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000004
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-514"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000005
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-514"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000006
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-520"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-514"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000007
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-528"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-520"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-514"
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000008
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-514"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-520"
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-528"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000009
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-528"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-528"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000010
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-528"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-528"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-528"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-528"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000011
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000012
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000013
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000014
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000015
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000016
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000016
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000017
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000018
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000019
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000020
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000021
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000022
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000023
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000024
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-513"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000025
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-514"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000026
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-516"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000027
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-520"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000028
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-528"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000029
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-528"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-528"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000030
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Primitive-513"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-513"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000031
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000001"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000032
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000002"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000033
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000003"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000034
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000004"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000035
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000005"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000036
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000006"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000037
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000007"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000038
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000008"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000039
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000009"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000040
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000010"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000041
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000008"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-528"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000042
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000008"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-520"
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000043
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000003"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000044
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000007"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-513"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Primitive-516"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000045
    ABI:             Invalid
    ReturnType:
      UnqualifiedType: "/Types/Struct-7000000010"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
      - Index:           3
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_64_IDs{
  1000000001, 1000000006, 1000000010, 1000000011, 1000000012, 1000000014,
  1000000023, 1000000024, 1000000025, 1000000026,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
constexpr const char *SystemV_x86_64 = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              513
    PrimitiveKind:   Generic
    Size:            1
  - !Primitive
    Kind:            Primitive
    ID:              514
    PrimitiveKind:   Generic
    Size:            2
  - !Primitive
    Kind:            Primitive
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              520
    PrimitiveKind:   Generic
    Size:            8
  - !Primitive
    Kind:            Primitive
    ID:              528
    PrimitiveKind:   Generic
    Size:            16
  - !Primitive
    Kind:            Primitive
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
  - !Primitive
    Kind:            Primitive
    ID:              784
    PrimitiveKind:   PointerOrNumber
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000001
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000002
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          1
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000003
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000004
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000005
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000006
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000007
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000008
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          12
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          20
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000009
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000010
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          48
    Size:            64
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000006
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000010
    ABI:             SystemV_x86_64
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
    ID:              1000000011
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000012
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           5
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000014
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000023
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-784"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000024
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-784"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000025
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-784"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000001
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000002
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000003
    Arguments:
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000017
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000019
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000020
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000023
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000027
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000029
    Arguments:
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000041
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x64_IDs{
  1000000001, 1000000004, 1000000014, 1000000019, 1000000020, 1000000021,
  1000000022,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
constexpr const char *Microsoft_x64 = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              513
    PrimitiveKind:   Generic
    Size:            1
  - !Primitive
    Kind:            Primitive
    ID:              514
    PrimitiveKind:   Generic
    Size:            2
  - !Primitive
    Kind:            Primitive
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              520
    PrimitiveKind:   Generic
    Size:            8
  - !Primitive
    Kind:            Primitive
    ID:              528
    PrimitiveKind:   Generic
    Size:            16
  - !Primitive
    Kind:            Primitive
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000001
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000002
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          1
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000003
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000004
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000005
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000006
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000007
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000008
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          12
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          20
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000009
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000010
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          48
    Size:            64
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000004
    ABI:             Microsoft_x64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000014
    ABI:             Microsoft_x64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000019
    ABI:             Microsoft_x64
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
    ID:              1000000020
    ABI:             Microsoft_x64
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
    ID:              1000000021
    ABI:             Microsoft_x64
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
    ID:              1000000022
    ABI:             Microsoft_x64
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
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000001
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000002
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000003
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000017
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000019
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000020
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000023
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000027
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000041
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x64_vectorcall_IDs{
  1000000001, 1000000004, 1000000014, 1000000019, 1000000020, 1000000021,
  1000000022,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
constexpr const char *Microsoft_x64_vectorcall = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              513
    PrimitiveKind:   Generic
    Size:            1
  - !Primitive
    Kind:            Primitive
    ID:              514
    PrimitiveKind:   Generic
    Size:            2
  - !Primitive
    Kind:            Primitive
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              520
    PrimitiveKind:   Generic
    Size:            8
  - !Primitive
    Kind:            Primitive
    ID:              528
    PrimitiveKind:   Generic
    Size:            16
  - !Primitive
    Kind:            Primitive
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000001
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000002
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          1
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000003
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000004
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000005
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000006
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000007
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000008
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          12
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          20
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000009
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000010
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          48
    Size:            64
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x64_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000004
    ABI:             Microsoft_x64_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000014
    ABI:             Microsoft_x64_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000019
    ABI:             Microsoft_x64_vectorcall
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
    ID:              1000000020
    ABI:             Microsoft_x64_vectorcall
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
    ID:              1000000021
    ABI:             Microsoft_x64_vectorcall
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
    ID:              1000000022
    ABI:             Microsoft_x64_vectorcall
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
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000001
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000002
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000003
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000017
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000019
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000020
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000023
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000027
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000041
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            8
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x64_clrcall_IDs{
  1000000001,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023
};
constexpr const char *Microsoft_x64_clrcall = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
  - !Primitive
    Kind:            Primitive
    ID:              513
    PrimitiveKind:   Generic
    Size:            1
  - !Primitive
    Kind:            Primitive
    ID:              514
    PrimitiveKind:   Generic
    Size:            2
  - !Primitive
    Kind:            Primitive
    ID:              516
    PrimitiveKind:   Generic
    Size:            4
  - !Primitive
    Kind:            Primitive
    ID:              520
    PrimitiveKind:   Generic
    Size:            8
  - !Primitive
    Kind:            Primitive
    ID:              528
    PrimitiveKind:   Generic
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000001
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000002
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          1
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000003
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000004
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-514"
        Offset:          2
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000005
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              7000000006
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
    Size:            16
  - !Struct
    Kind:            Struct
    ID:              7000000007
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000008
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-516"
        Offset:          4
      - Type:
          UnqualifiedType: "/Types/Primitive-513"
        Offset:          8
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          12
      - Type:
          UnqualifiedType: "/Types/Primitive-520"
        Offset:          20
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000009
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
    Size:            32
  - !Struct
    Kind:            Struct
    ID:              7000000010
    Fields:
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          0
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          16
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          32
      - Type:
          UnqualifiedType: "/Types/Primitive-528"
        Offset:          48
    Size:            64
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000001
    ABI:             Microsoft_x64_clrcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:       []
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000001
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000002
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000003
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000017
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000019
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000020
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000023
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - rsi_x86_64
      - rdi_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
      - xmm6_x86_64
      - xmm7_x86_64
    FinalStackOffset: 80
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

} // namespace ABI_TEST
