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

inline const SortedVector<size_t> SystemV_x86_IDs{
  1000000001, 1000000011, 1000000015,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
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
    FinalStackOffset: 16
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
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
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
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 96
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 96
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_regparm_1_IDs{
  1000000001, 1000000002, 1000000011, 1000000015, 1000000019,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
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
    ID:              1000000015
    ABI:             SystemV_x86_regparm_1
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
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
    FinalStackOffset: 16
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
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
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
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 96
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 96
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_regparm_2_IDs{
  1000000001, 1000000002, 1000000006, 1000000007, 1000000011, 1000000015,
  1000000017, 1000000019,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
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
    ID:              1000000015
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000017
    ABI:             SystemV_x86_regparm_2
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
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
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> SystemV_x86_regparm_3_IDs{
  1000000001, 1000000002, 1000000006, 1000000007, 1000000010, 1000000011,
  1000000015, 1000000017, 1000000019,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
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
    ID:              1000000015
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:       []
  - !CABIFunctionType
    Kind:            CABIFunctionType
    ID:              1000000017
    ABI:             SystemV_x86_regparm_3
    ReturnType:
      UnqualifiedType: "/Types/Primitive-776"
    Arguments:
      - Index:           0
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Index:           1
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
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
    FinalStackOffset: 80
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
      - Location:        edx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_cdecl_IDs{
  1000000001, 1000000011, 1000000015,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
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
    FinalStackOffset: 4
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
    FinalStackOffset: 8
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
    FinalStackOffset: 8
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
    FinalStackOffset: 12
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 20
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 40
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_stdcall_IDs{
  1000000001, 1000000011, 1000000015,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
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
    FinalStackOffset: 4
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
    FinalStackOffset: 8
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
    FinalStackOffset: 8
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
    FinalStackOffset: 12
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 20
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 40
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_thiscall_IDs{
  1000000001, 1000000004, 1000000011, 1000000015,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
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
    FinalStackOffset: 4
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
    FinalStackOffset: 8
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
    FinalStackOffset: 8
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
    FinalStackOffset: 12
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 20
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 40
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_fastcall_IDs{
  1000000001, 1000000004, 1000000011, 1000000015, 1000000018,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
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
      UnqualifiedType: "/Types/Primitive-776"
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
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 4
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 4
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 12
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 60
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 28
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 68
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_clrcall_IDs{
  1000000001,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023
};
constexpr const char *Microsoft_x86_clrcall = R"(---
Architecture:    x86
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
    ABI:             Microsoft_x86_clrcall
    ReturnType:
      UnqualifiedType: "/Types/Primitive-256"
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
    FinalStackOffset: 4
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
    FinalStackOffset: 8
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
    FinalStackOffset: 8
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
    FinalStackOffset: 12
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 20
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
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
    FinalStackOffset: 64
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

inline const SortedVector<size_t> Microsoft_x86_vectorcall_IDs{
  1000000001, 1000000004, 1000000011, 1000000018,

  2000000001, 2000000002, 2000000003, 2000000004, 2000000005, 2000000006,
  2000000007, 2000000008, 2000000009, 2000000010, 2000000011, 2000000012,
  2000000013, 2000000014, 2000000015, 2000000016, 2000000017, 2000000018,
  2000000019, 2000000020, 2000000021, 2000000022, 2000000023, 2000000024,
  2000000025, 2000000026, 2000000027, 2000000028, 2000000029, 2000000030,
  2000000031, 2000000032, 2000000033, 2000000034, 2000000035, 2000000036,
  2000000037, 2000000038, 2000000039, 2000000040, 2000000041, 2000000042,
  2000000043, 2000000044, 2000000045
};
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
    ID:              772
    PrimitiveKind:   PointerOrNumber
    Size:            4
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
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
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
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 4
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000004
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 4
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000005
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000006
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000007
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000008
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000009
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000010
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000011
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000012
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000013
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000014
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000015
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 8
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000016
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
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
    FinalStackOffset: 32
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000018
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
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
    FinalStackOffset: 32
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000021
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 72
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000022
    Arguments:       []
    ReturnValues:    []
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 48
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
    FinalStackOffset: 64
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000024
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000025
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000026
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
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
    ID:              2000000027
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-520"
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
    ID:              2000000028
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
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
    ID:              2000000029
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-528"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 16
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000030
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 12
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000031
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000001"
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
    ID:              2000000032
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000002"
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
    ID:              2000000033
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
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
    ID:              2000000034
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000004"
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
    ID:              2000000035
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000005"
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
    ID:              2000000036
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000006"
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
    ID:              2000000037
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
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
    ID:              2000000038
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
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
    ID:              2000000039
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000009"
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
    ID:              2000000040
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
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
    ID:              2000000041
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 60
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000042
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000008"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 28
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000043
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000003"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 36
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000044
    Arguments:
      - Location:        ecx_x86
        Type:
          UnqualifiedType: "/Types/Primitive-772"
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000007"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 68
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              2000000045
    Arguments:       []
    ReturnValues:
      - Location:        eax_x86
        Type:
          UnqualifiedType: "/Types/Struct-7000000010"
          Qualifiers:
            - Kind:            Pointer
              Size:            4
    PreservedRegisters:
      - ebx_x86
      - esi_x86
      - edi_x86
      - ebp_x86
      - esp_x86
    FinalStackOffset: 32
Segments:        []
Functions:       []
ImportedDynamicFunctions: []
...
)";

} // namespace ABI_TEST
