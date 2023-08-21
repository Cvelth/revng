#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <optional>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

#include "revng/ADT/SortedVector.h"
#include "revng/Model/Architecture.h"
#include "revng/Model/Register.h"

namespace abi::runtime_test {

struct Argument {
  llvm::StringRef Type;
  std::vector<std::byte> Bytes;
  uint64_t MaybePointer;
};

struct RawRegister {
  llvm::StringRef Name;
  uint64_t Value;
  std::vector<std::byte> Bytes;
};

struct Register {
  model::Register::Values Name;
  uint64_t Value;
  std::vector<std::byte> Bytes;

  Register(model::Register::Values Name,
           uint64_t Value = 0,
           std::vector<std::byte> Bytes = {}) :
    Name(Name), Value(Value), Bytes(Bytes) {}

  Register(RawRegister &&Raw, model::Architecture::Values Architecture) :
    Name(model::Register::fromRegisterName(Raw.Name, Architecture)),
    Value(Raw.Value),
    Bytes(std::move(Raw.Bytes)) {}

  Register(const Register &) = default;
  Register(Register &&) = default;
  Register &operator=(const Register &) = default;
  Register &operator=(Register &&) = default;
};

} // namespace abi::runtime_test

template<>
struct KeyedObjectTraits<abi::runtime_test::Register> {
  static model::Register::Values
  key(const abi::runtime_test::Register &Object) {
    return Object.Name;
  }
  static abi::runtime_test::Register
  fromKey(const model::Register::Values &Key) {
    return { Key };
  }
};

namespace abi::runtime_test {

struct RawState {
  std::vector<RawRegister> Registers;
  std::vector<std::byte> Stack;
  RawRegister StackPointer;

  bool empty() const { return Registers.empty() && Stack.empty(); }
};

struct State {
  SortedVector<Register> Registers;
  std::vector<std::byte> Stack;
  std::optional<Register> StackPointer;

  State(RawState &&Raw, model::Architecture::Values Arch) : Stack(Raw.Stack) {
    Registers.reserve(Raw.Registers.size());
    for (auto I = Registers.batch_insert(); auto &RawRegister : Raw.Registers)
      I.emplace(std::move(RawRegister), Arch);
    if (!Raw.StackPointer.Name.empty())
      StackPointer = Register(std::move(Raw.StackPointer), Arch);
  }

  std::optional<llvm::ArrayRef<std::byte>>
  operator[](model::Register::Values Register) const {
    auto Iterator = Registers.find(Register);
    if (Iterator != Registers.end())
      return Iterator->Bytes;
    else
      return std::nullopt;
  }
};

struct ArgumentTest {
  llvm::StringRef Name;
  State StateBeforeTheCall;
  std::vector<Argument> Arguments;

  ArgumentTest(llvm::StringRef Name,
               State StateBeforeTheCall,
               std::vector<Argument> Arguments) :
    Name(Name), StateBeforeTheCall(StateBeforeTheCall), Arguments(Arguments) {}
};

struct ReturnValueTest {
  llvm::StringRef Function;
  State StateAfterTheCall;
  State StateAfterTheReturn;
  Argument ReturnValueAddress;
  Argument ReturnValue;
  Argument ExpectedReturnValue;

  ReturnValueTest(llvm::StringRef Function,
                  State StateAfterTheCall,
                  State StateAfterTheReturn,
                  Argument ReturnValueAddress,
                  Argument ReturnValue,
                  Argument ExpectedReturnValue) :
    Function(Function),
    StateAfterTheCall(StateAfterTheCall),
    StateAfterTheReturn(StateAfterTheReturn),
    ReturnValueAddress(ReturnValueAddress),
    ReturnValue(ReturnValue),
    ExpectedReturnValue(ExpectedReturnValue) {}
};

template<typename Iteration>
struct Named {
  llvm::StringRef Name;
  std::vector<Iteration> Iterations = {};
  const std::vector<Iteration> &operator*() const { return Iterations; }
};

} // namespace abi::runtime_test

template<typename Iteration>
struct KeyedObjectTraits<abi::runtime_test::Named<Iteration>> {
  using Type = abi::runtime_test::Named<Iteration>;
  static llvm::StringRef key(const Type &Object) { return Object.Name; }
  static Type fromKey(const llvm::StringRef &Key) { return { Key }; }
};

namespace abi::runtime_test {

struct Parsed {
  llvm::StringRef Architecture;
  bool IsLittleEndian = false;
  SortedVector<Named<ArgumentTest>> ArgumentTests;
  SortedVector<Named<ReturnValueTest>> ReturnValueTests;
};

Parsed parse(llvm::StringRef RuntimeArtifact,
             model::Architecture::Values Architecture);

} // namespace abi::runtime_test
