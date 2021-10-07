//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

namespace ABI_TEST_DATA {

namespace success {

const char *Microsoft_x64 = R"(---
Architecture: x86_64
Segments:
Functions:
ImportedDynamicFunctions:
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
    ID:              9385395912304380693
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              9385395912304380694
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
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
...
)";

} // namespace success

namespace failure {

const char *Microsoft_x64 = R"(---
Architecture: x86_64
Segments:
Functions:
ImportedDynamicFunctions:
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
    ID:              9385395912304380693
    Arguments:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues:
      - Location:        rax_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    PreservedRegisters: []
    FinalStackOffset: 0
  - !RawFunctionType
    Kind:            RawFunctionType
    ID:              9385395912304380694
    Arguments:
      - Location:        rcx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        rdx_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
      - Location:        r15_x86_64
        Type:
          UnqualifiedType: "/Types/Primitive-516"
    ReturnValues: []
    PreservedRegisters: []
    FinalStackOffset: 0
...
)";

} // namespace failure

} // namespace ABI_TEST_DATA
