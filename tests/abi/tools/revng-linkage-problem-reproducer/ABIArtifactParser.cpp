/// \file Verify.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <algorithm>

#include "revng/ABI/FunctionType.h"

#include "ABIArtifactParser.h"

namespace State {

using namespace abi::artifact;

struct SingleRun {
  llvm::StringRef Function;
  Registers Registers;
  Stack Stack;
  Arguments ReturnValue;
  Arguments Arguments;
};
using Deserialized = llvm::SmallVector<SingleRun, 16>;

} // namespace State

//
// `llvm::yaml` traits for the internal state.
//

template<>
struct llvm::yaml::MappingTraits<State::Register> {
  static void mapping(IO &IO, State::Register &N) {
    IO.mapRequired("Name", N.Name);
    IO.mapRequired("Value", N.Value);
  }
};

template<>
struct llvm::yaml::MappingTraits<State::StackValue> {
  static void mapping(IO &IO, State::StackValue &N) {
    IO.mapRequired("Offset", N.Offset);
    IO.mapRequired("Value", N.Value);
  }
};

template<>
struct llvm::yaml::MappingTraits<State::Argument> {
  static void mapping(IO &IO, State::Argument &N) {
    IO.mapRequired("Type", N.Type);
    IO.mapRequired("Value", N.Value);
    IO.mapRequired("Address", N.Address);
  }
};

template<>
struct llvm::yaml::MappingTraits<State::SingleRun> {
  static void mapping(IO &IO, State::SingleRun &N) {
    IO.mapRequired("Function", N.Function);
    IO.mapRequired("Registers", N.Registers);
    IO.mapRequired("Stack", N.Stack);
    IO.mapRequired("Arguments", N.Arguments);
    IO.mapRequired("ReturnValue", N.ReturnValue);
  }
};

template<typename Type>
struct GenericSmallVectorYamlTrait {
  static size_t size(llvm::yaml::IO &, Type &Value) { return Value.size(); }
  static auto &element(llvm::yaml::IO &, Type &Value, size_t Index) {
    if (Index >= Value.size())
      Value.resize(Index + 1);
    return Value[Index];
  }
};

template<>
struct llvm::yaml::SequenceTraits<State::Registers>
  : GenericSmallVectorYamlTrait<State::Registers> {};
template<>
struct llvm::yaml::SequenceTraits<State::Stack>
  : GenericSmallVectorYamlTrait<State::Stack> {};
template<>
struct llvm::yaml::SequenceTraits<State::Arguments>
  : GenericSmallVectorYamlTrait<State::Arguments> {};
template<>
struct llvm::yaml::SequenceTraits<State::Deserialized>
  : GenericSmallVectorYamlTrait<State::Deserialized> {};

static State::ModelRegisters
toModelRegisters(const State::Registers &Input,
                 model::Architecture::Values Arch) {
  State::ModelRegisters Result;

  for (const State::Register &Reg : Input) {
    auto &Output = Result[model::Register::fromRegisterName(Reg.Name, Arch)];
    Output.Value = Reg.Value;
  }

  return Result;
}

static State::Iteration toIteration(const State::SingleRun &SingleRun,
                                    model::Architecture::Values Architecture) {
  revng_assert(SingleRun.ReturnValue.size() <= 1);
  return State::Iteration{ toModelRegisters(SingleRun.Registers, Architecture),
                           SingleRun.Stack,
                           SingleRun.Arguments,
                           SingleRun.ReturnValue.empty() ?
                             State::Argument{ "void", "", 0 } :
                             SingleRun.ReturnValue[0] };
}

State::Parsed abi::artifact::parse(llvm::StringRef RuntimeArtifact,
                                   model::Architecture::Values Architecture) {
  llvm::yaml::Input Reader(RuntimeArtifact);

  State::Deserialized Deserialized;
  Reader >> Deserialized;
  revng_assert(!Reader.error());

  State::Parsed Result;
  for (const State::SingleRun &SingleRun : Deserialized) {
    const State::Iteration &Iteration = toIteration(SingleRun, Architecture);
    Result[SingleRun.Function].Iterations.emplace_back(Iteration);
  }

  return Result;
}
