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
      - Location:        rsi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdi_x86_64
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
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000014
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
    ID:              1000000015
    Arguments: []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000016
    Arguments: []
    ReturnValues:
      - Location:        rbx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000017
    Arguments: []
    ReturnValues:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000018
    Arguments: []
    ReturnValues:
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000019
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
    ID:              1000000020
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
    ID:              1000000021
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
    ReturnValues:    []
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              1000000023
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
    ID:              1000000024
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
    ID:              1000000025
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
    ID:              1000000026
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
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000002
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000003
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
    ID:              2000000004
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000005
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000006
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
    ID:              2000000007
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
    ID:              2000000008
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
      - Index:           6
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000009
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000010
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000011
    ABI:             SystemV_x86_64
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
    ID:              2000000012
    ABI:             SystemV_x86_64
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
    ID:              2000000013
    ABI:             SystemV_x86_64
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000014
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000015
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           5
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000016
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           5
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           6
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
    ID:              2000000017
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments: []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000018
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000019
    ABI:             SystemV_x86_64
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
    ID:              2000000020
    ABI:             SystemV_x86_64
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
    ID:              2000000021
    ABI:             SystemV_x86_64
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
    ID:              2000000022
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000023
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           5
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000024
    ABI:             SystemV_x86_64
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
      - Index:           4
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           5
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Index:           6
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000025
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000026
    ABI:             SystemV_x86_64
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
    ID:              2000000027
    ABI:             SystemV_x86_64
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
    ID:              9000000028
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
    ID:              2000000028
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000028"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000029
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000028"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000000"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000030
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000028"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000028"
      - Index:           2
        Type:
          UnqualifiedType: "/Types/Struct-9000000028"
  - !Struct
    Kind:            Struct
    ID:              9000000031
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
    ID:              2000000031
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000000"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000031"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000032
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000031"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Struct-9000000031"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000033
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000031"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              2000000034
    ABI:             SystemV_x86_64
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
    ID:              2000000035
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Struct-9000000028"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-776"
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_64_IDs{
  1000000001, 1000000006, 1000000010, 1000000011, 1000000012, 1000000013,
  1000000015, 1000000024, 1000000025, 1000000026,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035
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
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
  - !Struct
    Kind:            Struct
    ID:              9000000024
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
    ID:              9000000025
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
    ID:              9000000026
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000013
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
    ID:              1000000015
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000024
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000024"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000025
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000025"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000026
    ABI:             SystemV_x86_64
    ReturnType:
      UnqualifiedType: "/Types/Struct-9000000026"
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
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
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
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
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
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
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
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
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
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
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
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
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
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
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
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
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000017
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000019
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000020
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
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
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
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
      - Location:        r8_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
      - Location:        r9_x86_64
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
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
      - Location:        r9_x86_64
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000025
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
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000026
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000027
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
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000028
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000029
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
      - Location:        r9_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
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
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000032
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0  
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000033
    Arguments:
      - Location:        rdi_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-776"
    ReturnValues:    []
    PreservedRegisters:
      - rbx_x86_64
      - rbp_x86_64
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000034
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
      - r12_x86_64
      - r13_x86_64
      - r14_x86_64
      - r15_x86_64
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000035
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
  1000000001, 1000000004, 1000000015, 1000000020, 1000000021, 1000000022,
  1000000023,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000025, 2000000026,
  2000000027, 2000000028, 2000000029, 2000000030, 2000000032, 2000000033,
  2000000034, 2000000035
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
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
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
    ID:              1000000015
    ABI:             Microsoft_x64
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000023
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
      - Location:        r9_x86_64
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
      - Location:        r9_x86_64
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
      - Location:        r9_x86_64
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
      - Location:        r9_x86_64
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
    ID:              2000000009
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
    ID:              2000000010
    Arguments:
      - Location:        rcx_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
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
    ID:              2000000012
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
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
      - Location:        r9_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
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
      - Location:        r9_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
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
      - Location:        r9_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
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
      - Location:        r9_x86_64
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
    ID:              2000000029
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
    ID:              2000000030
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
    ID:              2000000032
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
    ID:              2000000033
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
    ID:              2000000034
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
    ID:              2000000035
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
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x64_vectorcall_IDs{
  1000000001, 1000000004, 1000000015, 1000000020, 1000000021, 1000000022,
  1000000023,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000025, 2000000026,
  2000000027, 2000000028, 2000000029, 2000000030, 2000000032, 2000000033,
  2000000034, 2000000035
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
    ID:              776
    PrimitiveKind:   PointerOrNumber
    Size:            8
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
    ID:              1000000015
    ABI:             Microsoft_x64_vectorcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
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
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000023
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
      - Location:        r9_x86_64
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
      - Location:        r9_x86_64
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
      - Location:        r9_x86_64
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
      - Location:        r9_x86_64
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
    ID:              2000000009
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
    ID:              2000000010
    Arguments:
      - Location:        rcx_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
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
    ID:              2000000012
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
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
      - Location:        r9_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
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
      - Location:        r9_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
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
      - Location:        r9_x86_64
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
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
      - Location:        r9_x86_64
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
    ID:              2000000029
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
    ID:              2000000030
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
    ID:              2000000032
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
    ID:              2000000033
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
    ID:              2000000034
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
    ID:              2000000035
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
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x64_clrcall_IDs{
  1000000001,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005,
  2000000006, 2000000007, 2000000008, 2000000026, 2000000028,
  2000000029, 2000000032, 2000000033, 2000000034, 2000000035
};
constexpr const char *Microsoft_x64_clrcall = R"(---
Architecture:    x86_64
Types:
  - !Primitive
    Kind:            Primitive
    ID:              256
    PrimitiveKind:   Void
    Size:            0
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
    FinalStackOffset: 0
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
    FinalStackOffset: 0
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
    FinalStackOffset: 0
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
    FinalStackOffset: 0
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
    FinalStackOffset: 0
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
    FinalStackOffset: 0
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
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000026
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
    ID:              2000000028
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
    ID:              2000000029
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
    ID:              2000000032
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
    ID:              2000000033
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
    ID:              2000000034
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
    ID:              2000000035
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
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

} // namespace ABI_TEST
