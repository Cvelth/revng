#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <vector>

#include "revng/ABI/ScalarType.h"
#include "revng/ADT/SortedVector.h"
#include "revng/Model/ABI.h"
#include "revng/Model/RawFunctionType.h"
#include "revng/Model/Register.h"
#include "revng/Support/Debug.h"
#include "revng/TupleTree/TupleTree.h"
#include "revng/TupleTree/TupleTreeDiff.h"

/* TUPLE-TREE-YAML

name: Definition
type: struct
fields:
  - name: ABI
    doc: |
      Indicates the model::ABI this definition is for.
    type: model::ABI::Values

  - name: ArgumentsArePositionBased
    doc: |
      States whether the ABI is focused on preserving the argument order.

      Here's an example function:
      ```
      struct Big; // Is really big, so it can only be passed in memory.
      void function(Big, signed, float, unsigned);
      ```

      If the ABI is position based, the arguments would be passed in
      - memory for the `Big` argument (and a pointer to it in the first GPR).
      - the second GPR register for the `signed` argument.
      - the third vector register for the `float` argument.
      - the forth GPR register for the `unsigned` argument.

      If the ABI is NOT position based, the arguments would be passed in
      - memory for the `Big` argument.
      - the first GPR register for the `signed` argument.
      - the first vector register for the `float` argument.
      - the second GPR register for the `unsigned` argument.

      A typical example of a position-based ABI is `Microsoft_x86_64`,
      a non-position-based one - `SystemV_x86_64`.
    type: bool

  - name: OnlyStartDoubleArgumentsFromAnEvenRegister
    doc: |
      States whether an object that needs two GPRs to fit (5-8 bytes on 32-bit
      architectures and 9-16 bytes on 64-bit systems) is only allowed to start
      from a register with an even index.

      Here's an example function:
      ```
      void function(uint32_t, uint64_t);
      ```
      On a system with 32-bit wide GPRs, the first argument (`uint32_t`) is
      passed using the first allowed GPR (say `r0`).

      The second argument (`uint64_t`) requires two register to fit, so it's
      passed using second and third registers if the ABI allows starting double
      arguments from any register (`r1` and `r2` in this example), or third and
      forth if it only allows starting them from even registers (`r2` and `r3`
      in this example, since `r1` is considered an odd register (the count
      starts from 0, much like C array indexing).

      \note this option is only applicable for non-position based ABIs
      (if `ArgumentsArePositionBased` is `false`).
    type: bool

  - name: ArgumentsCanBeSplitBetweenRegistersAndStack
    doc: |
      States whether the ABI allows a single object that wouldn't fit into
      a single GPR (9+ bytes on 32-bit systems and 17+ bytes on 64-bit ones)
      to be partially passed in registers with the remainder placed on the stack
      if there are not enough registers to fit the entirety of it.

      As an example, let's say that there is a big object of type `Big` such
      that `sizeof(Big)` is equal to 16 bytes. On 32-bit system it would mean
      having to use four GPRs (`16 == 4 * 4`) to fit it.

      Let's look at an ABI that allocates four registers for passing function
      arguments (`r0-r3`). Then, for a function like
      ```
      void function(uint32_t, Big);
      ```
      the `uint32_t` argument would be passed in the first GPR (`r0`).
      But that would also mean that the remaining three available GPRs are not
      enough to fit the entirety of the `Big` object, meaning it needs to
      either be split between the registers and the memory, or passed using
      the stack. That's exactly what this option states.

      \note this option is only applicable for non-position based ABIs
      (if `ArgumentsArePositionBased` is `false`).
    type: bool

  - name: UsePointerToCopyForStackArguments
    doc:
      States how the stack arguments are passed.
      If `UsePointerToCopyForStackArguments` is true, pointers-to-copy are used,
      otherwise - the whole argument is copied onto the stack.

      \note this only affects the arguments with size exceeding the size of
      a single stack "slot" (which is equal to the GPR size for the architecture
      in question).
    type: bool

  - name: CalleeIsResponsibleForStackCleanup
    doc: |
      Specifies who is responsible for cleaning the stack after the function
      call. If equal to `true`, it's the callee, otherwise it the caller.
    type: bool

  - name: StackAlignment
    doc: |
      States the required alignment of the stack at the point of a function
      call in bytes.

      \note states minimum value for ABIs supporting multiple different stack
      alignment values, for example, if the ABI requires the stack to be aligned
      on 4 bytes for internal calls but on 8 bytes for interfaces (like 32-bit
      ARM ABI), the value of `StackAlignment` should be equal to 4.
    type: uint64_t

  - name: MaximumGPRsPerAggregateArgument
    doc: |
      States the maximum number of GPRs available to pass a single aggregate
      (a struct, a union, etc.) argument, meaning that it can only be passed in
      the GPRs if `MaximumGPRsPerAggregateArgument` is less than or equal to
      the number of the registers required to fit the object including padding.

      \note If `MaximumGPRsPerAggregateArgument` is equal to 0, it means that
      the ABI does not allow aggregate arguments to use GPRs.

      \note If an argument doesn't fit into the specified registers or
      uses irregular padding, the registers are not used and the object is
      passed using the memory (stack, pointer-to-copy, etc.).
    type: uint64_t

  - name: MaximumGPRsPerAggregateReturnValue
    doc: |
      States the maximum number of GPRs available to return a single aggregate
      (a struct, a union, etc.) value, meaning that it can only be returned
      in the GPRs if `MaximumGPRsPerAggregateReturnValue` is less than or equal
      to the number of the registers required to fit the object including
      padding.

      \note If `MaximumGPRsPerAggregateReturnValue` is equal to 0, it means
      that the ABI does not allow aggregate return values to use GPRs.

      \note If a return value doesn't fit into the specified registers or
      uses irregular padding, the registers are not used and the object is
      passed using the memory (stack, pointer-to-copy, etc.).
    type: uint64_t

  - name: MaximumGPRsPerScalarArgument
    doc: |
      States the maximum number of GPRs available to pass a single scalar
      (`int`, `__int128`, pointer, etc.) argument, meaning that it can only be
      passed in the GPRs if `MaximumGPRsPerScalarArgument` is less than or
      equal to the number of the registers required to fit the object.

      \note If `MaximumGPRsPerScalarArgument` is equal to 0, it means that
      the ABI does not allow scalar arguments to use GPRs.

      \note If an argument doesn't fit into the specified registers or
      uses irregular padding, the registers are not used and the object is
      passed using the memory (stack, pointer-to-copy, etc.).
    type: uint64_t

  - name: MaximumGPRsPerScalarReturnValue
    doc: |
      States the maximum number of GPRs available to return a single scalar
      (`int`, `__int128`, pointer, etc.) value, meaning that it can only be
      returned in the GPRs if `MaximumGPRsPerScalarReturnValue` is less than
      or equal to the number of the registers required to fit the object
      including padding.

      \note If `MaximumGPRsPerScalarReturnValue` is equal to 0, it means
      that the ABI does not allow scalar return values to use GPRs.

      \note If a return value doesn't fit into the specified registers or
      uses irregular padding, the registers are not used and the object is
      passed using the memory (stack, pointer-to-copy, etc.).
    type: uint64_t

  - name: GeneralPurposeArgumentRegisters
    doc: |
      Stores the list of general purpose registers allowed to be used for
      passing arguments and the order they are to be used in.
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: GeneralPurposeReturnValueRegisters
    doc: |
      Stores the list of general purpose registers allowed to be used for
      returning values and the order they are to be used in.
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: VectorArgumentRegisters
    doc: |
      Stores the list of vector registers allowed to be used for passing
      arguments and the order they are to be used in.
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: VectorReturnValueRegisters
    doc: |
      Stores the list of vector registers allowed to be used for returning
      values and the order they are to be used in.
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true
  - name: CalleeSavedRegisters
    doc: |
      Stores the list of registers for which the ABI requires the callee to
      preserve the value, meaning that when the callee returns, the value of
      those registers must be the same as it was when the function was called.
    sequence:
      type: std::vector
      elementType: model::Register::Values
    optional: true

  - name: ReturnValueLocationRegister
    doc: |
      Specifies a register to be used for returning (or even passing,
      depending on ABI) the pointer to the memory used for returning
      copies of big aggregate objects.

      Can be `model::Register::Invalid` for ABIs that do not support returning
      values by 'pointer-to-copy'.
    type: model::Register::Values
    optional: true

  - name: ReturnValueLocationOnStack
    doc: |
      Specifies whether stack is used to pass the return value location.

      This is only relevant if `ReturnValueLocationRegister` is set to `Invalid`
    type: bool
    optional: true

  - name: ScalarTypes
    doc: |
      This provides a way to introduce some type-specific constraint information
      to ABI definition, e.g. how types get aligned based on their size.
    sequence:
      type: SortedVector
      elementType: ScalarType

  - name: FloatingPointScalarTypes
    doc: |
      This provides a way to introduce some type-specific constraint information
      to ABI definition, e.g. how types get aligned based on their size.
    sequence:
      type: SortedVector
      elementType: ScalarType

key:
  - ABI

TUPLE-TREE-YAML */

#include "revng/ABI/Generated/Early/Definition.h"

namespace abi {

class Definition : public generated::Definition {
public:
  using generated::Definition::Definition;

public:
  static const Definition &get(model::ABI::Values ABI);

public:
  std::string_view getName() const { return model::ABI::getName(ABI()); }

  /// Make sure current definition is valid.
  bool verify() const debug_function;

  /// Checks whether a given function data does not contradict this ABI
  ///
  /// \note this is not an exhaustive check, so if it returns `false`,
  /// the function definitely is NOT compatible, but if it returns `true`
  /// it might either be compatible or not.
  ///
  /// \note this also asserts \ref isValid
  ///
  /// \tparam Register The type representing the registers, example of valid
  ///         values include \ref model::TypedRegister and
  ///         \ref model::NamedTypedRegister
  ///
  /// \param ArgumentRegisters The list of registers used for passing arguments
  ///        of the function in question
  /// \param ReturnValueRegisters The list of registers used for returning
  ///        values of the function in question
  ///
  /// \return `false` if the function is definitely NOT compatible with the ABI,
  ///         `true` if it might be compatible.
  bool isIncompatibleWith(const model::RawFunctionType &Function) const;

  /// Compute the natural alignment of the type in accordance with
  /// the current ABI
  ///
  /// \note  It mirrors, `model::Type::size()` pretty closely, see documentation
  ///        related to it (and usage of the coroutines inside this codebase
  ///        in general) for more details on how it works.
  ///
  /// \param Type The type to compute the alignment of.
  /// \param ABI The ABI used to determine alignment of the primitive components
  ///        of the type
  ///
  /// \return either an alignment or a `std::nullopt` when it's not applicable.
  inline std::optional<uint64_t>
  alignment(const model::QualifiedType &Type) const {
    model::VerifyHelper VH;
    return alignment(VH, Type);
  }

  std::optional<uint64_t>
  alignment(model::VerifyHelper &VH, const model::QualifiedType &Type) const;
};

} // namespace abi

#include "revng/ABI/Generated/Late/Definition.h"
