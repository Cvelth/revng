/// \file Verify.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <algorithm>

#include "revng/ABI/FunctionType/Layout.h"
#include "revng/Model/Binary.h"

#include "ABIRuntimeTestResultParser.h"

namespace abi::runtime_test {

struct SingleIteration {
  llvm::StringRef Function;

  RawState StateBeforeTheCall; // Argument tests only
  RawState StateAfterTheCall; // Return value tests only
  RawState StateAfterTheReturn; // Return value tests only

  std::vector<Argument> ReturnValueAddress; // Return value tests only
  std::vector<Argument> ReturnValue; // Return value tests only
  std::vector<Argument> ExpectedReturnValue; // Return value tests only
  std::vector<Argument> Arguments; // Argument tests only

  std::optional<ArgumentTest> asArgumentTest(model::Architecture::Values Arch) {
    if (!StateAfterTheCall.empty() || !StateAfterTheReturn.empty()
        || !ReturnValueAddress.empty() || !ReturnValue.empty()
        || !ExpectedReturnValue.empty()) {
      return std::nullopt;
    }

    return ArgumentTest(Function,
                        State(std::move(StateBeforeTheCall), Arch),
                        std::move(Arguments));
  }

  std::optional<ReturnValueTest>
  asReturnValueTest(model::Architecture::Values Arch) {
    if (!StateBeforeTheCall.empty() || !Arguments.empty())
      return std::nullopt;

    revng_assert(ReturnValueAddress.size() == 1);
    revng_assert(ReturnValue.size() == 1);
    revng_assert(ExpectedReturnValue.size() == 1);

    return ReturnValueTest(Function,
                           State(std::move(StateAfterTheCall), Arch),
                           State(std::move(StateAfterTheReturn), Arch),
                           std::move(ReturnValueAddress[0]),
                           std::move(ReturnValue[0]),
                           std::move(ExpectedReturnValue[0]));
  }
};

struct Deserialized {
  llvm::StringRef TargetArchitecture;
  bool IsLittleEndian;
  std::vector<SingleIteration> Iterations;
};

} // namespace abi::runtime_test

//
// `llvm::yaml` traits for the internal state.
// TODO: Consider using `TupleTreeGenerator` instead.
//

template<>
struct llvm::yaml::ScalarTraits<std::byte> {
  static_assert(sizeof(std::byte) == sizeof(uint8_t));

  static void output(const std::byte &Value, void *, llvm::raw_ostream &Out) {
    Out << uint8_t(Value);
  }

  static StringRef input(StringRef Scalar, void *Ptr, std::byte &Value) {
    uint8_t Temporary;
    auto Err = llvm::yaml::ScalarTraits<uint8_t>::input(Scalar, Ptr, Temporary);
    if (!Err.empty())
      return Err;

    Value = static_cast<std::byte>(Temporary);
    return StringRef{};
  }

  static QuotingType mustQuote(StringRef Scalar) {
    return llvm::yaml::ScalarTraits<uint8_t>::mustQuote(Scalar);
  }
};
LLVM_YAML_IS_FLOW_SEQUENCE_VECTOR(std::byte);

template<>
struct llvm::yaml::MappingTraits<abi::runtime_test::RawRegister> {
  static void mapping(IO &IO, abi::runtime_test::RawRegister &N) {
    IO.mapRequired("Name", N.Name);
    IO.mapRequired("Value", N.Value);
    IO.mapRequired("Bytes", N.Bytes);
  }
};
LLVM_YAML_IS_SEQUENCE_VECTOR(abi::runtime_test::RawRegister)

template<>
struct llvm::yaml::MappingTraits<abi::runtime_test::RawState> {
  static void mapping(IO &IO, abi::runtime_test::RawState &N) {
    IO.mapRequired("Registers", N.Registers);
    IO.mapRequired("Stack", N.Stack);
    IO.mapOptional("StackPointer", N.StackPointer);
  }
};
LLVM_YAML_IS_SEQUENCE_VECTOR(abi::runtime_test::RawState);

template<>
struct llvm::yaml::MappingTraits<abi::runtime_test::Argument> {
  static void mapping(IO &IO, abi::runtime_test::Argument &N) {
    IO.mapRequired("Type", N.Type);
    IO.mapRequired("Bytes", N.Bytes);
    IO.mapOptional("Pointer", N.MaybePointer);
  }
};
LLVM_YAML_IS_SEQUENCE_VECTOR(abi::runtime_test::Argument);

template<>
struct llvm::yaml::MappingTraits<abi::runtime_test::SingleIteration> {
  static void mapping(IO &IO, abi::runtime_test::SingleIteration &N) {
    IO.mapRequired("Function", N.Function);

    IO.mapOptional("StateBeforeTheCall", N.StateBeforeTheCall);
    IO.mapOptional("StateAfterTheCall", N.StateAfterTheCall);
    IO.mapOptional("StateAfterTheReturn", N.StateAfterTheReturn);

    IO.mapOptional("ReturnValueAddress", N.ReturnValueAddress);
    IO.mapOptional("ReturnValue", N.ReturnValue);
    IO.mapOptional("ExpectedReturnValue", N.ExpectedReturnValue);
    IO.mapOptional("Arguments", N.Arguments);
  }
};
LLVM_YAML_IS_SEQUENCE_VECTOR(abi::runtime_test::SingleIteration);

template<>
struct llvm::yaml::MappingTraits<abi::runtime_test::Deserialized> {
  static void mapping(IO &IO, abi::runtime_test::Deserialized &N) {
    IO.mapRequired("TargetArchitecture", N.TargetArchitecture);
    IO.mapRequired("IsLittleEndian", N.IsLittleEndian);
    IO.mapRequired("Iterations", N.Iterations);
  }
};

struct VerificationHelper {
  llvm::StringRef FunctionName;
  size_t ArgumentIterationCount = 0;
  size_t ReturnValueIterationCount = 0;
};

template<>
struct KeyedObjectTraits<VerificationHelper> {
  static llvm::StringRef key(const VerificationHelper &Object) {
    return Object.FunctionName;
  }
  static VerificationHelper fromKey(const llvm::StringRef &Key) {
    return { Key };
  }
};

static bool verify(const abi::runtime_test::Deserialized &Data,
                   bool ShouldAssert) {
  if (Data.Iterations.empty()) {
    revng_assert(!ShouldAssert);
    return false;
  }

  SortedVector<VerificationHelper> VH;
  for (const abi::runtime_test::SingleIteration &Run : Data.Iterations) {
    VerificationHelper &Helper = VH[Run.Function];
    if (!Run.Arguments.empty())
      ++Helper.ArgumentIterationCount;
    if (!Run.ReturnValue.empty())
      ++Helper.ReturnValueIterationCount;
  }

  if (VH.empty()) {
    revng_assert(!ShouldAssert);
    return false;
  }

  size_t ExpectedIterationCount = VH.begin()->ArgumentIterationCount;
  for (const VerificationHelper &Helper : VH) {
    bool IsAnArgumentTest = Helper.ArgumentIterationCount != 0;
    bool IsAnReturnValueTest = Helper.ReturnValueIterationCount != 0;
    if (IsAnArgumentTest == IsAnReturnValueTest) {
      if (IsAnArgumentTest)
        revng_assert(!ShouldAssert,
                     "Argument and return value tests must be separate.");
      else
        revng_assert(!ShouldAssert, "Tests must do something.");
    }

    std::size_t Sum = Helper.ArgumentIterationCount
                      + Helper.ReturnValueIterationCount;
    if (Sum != ExpectedIterationCount) {
      revng_assert(!ShouldAssert, "Iteration count is not consistent.");
      return false;
    }
  }

  return true;
}

abi::runtime_test::Parsed
abi::runtime_test::parse(llvm::StringRef RuntimeArtifact,
                         model::Architecture::Values Architecture) {
  llvm::yaml::Input Reader(RuntimeArtifact);

  abi::runtime_test::Deserialized Deserialized;
  Reader >> Deserialized;
  revng_assert(!Reader.error());

  llvm::StringRef ArchitectureName = model::Architecture::getName(Architecture);
  if (ArchitectureName != Deserialized.TargetArchitecture) {
    std::string Error = "Target architecture ('"
                        + Deserialized.TargetArchitecture.str()
                        + "') does not match the expected one: '"
                        + ArchitectureName.str() + "'\n";
    revng_abort(Error.c_str());
  }

  verify(Deserialized, true);

  abi::runtime_test::Parsed Result;
  Result.Architecture = Deserialized.TargetArchitecture;
  Result.IsLittleEndian = Deserialized.IsLittleEndian;

  for (auto &Iteration : Deserialized.Iterations) {
    if (auto ReturnValue = Iteration.asReturnValueTest(Architecture)) {
      revng_check(!Iteration.asArgumentTest(Architecture).has_value());
      auto &Is = Result.ReturnValueTests[Iteration.Function].Iterations;
      Is.emplace_back(std::move(ReturnValue.value()));
    } else if (auto Argument = Iteration.asArgumentTest(Architecture)) {
      auto &Is = Result.ArgumentTests[Iteration.Function].Iterations;
      Is.emplace_back(std::move(Argument.value()));
    } else {
      revng_abort("This test tests neither arguments nor return values.");
    }
  }

  return Result;
}
