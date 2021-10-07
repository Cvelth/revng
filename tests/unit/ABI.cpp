/// \file ABI.cpp
/// \brief ABI tests

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#define BOOST_TEST_MODULE ABI
bool init_unit_test();
#include "boost/test/unit_test.hpp"

#include "revng/Model/Binary.h"
#include "revng/StackAnalysis/ABI.h"

#include "helpers/ABI.h"

auto DefaultCallable = [](const std::optional<model::CABIFunctionType> &Input) {
  return Input.has_value();
};

static std::vector<const model::RawFunctionType *>
chooseRawFunctions(const SortedVector<UpcastablePointer<model::Type>> &Types) {
  std::vector<const model::RawFunctionType *> Result;
  for (const auto &Type : Types)
    if (auto *Function = llvm::dyn_cast<model::RawFunctionType>(Type.get()))
      Result.emplace_back(Function);
  return Result;
}

using CallableType = bool (*)(const std::optional<model::CABIFunctionType> &);
template<typename ABI, bool ShouldFail>
bool testImpl(const char *Source, CallableType Callable = DefaultCallable) {
  auto Deserialized = TupleTree<model::Binary>::deserialize(Source);
  BOOST_REQUIRE(Deserialized);
  auto &ModelBinary = **Deserialized;

  auto RawFunctions = chooseRawFunctions(ModelBinary.Types);
  for (auto *Function : RawFunctions) {
    auto Result = ABI::toCABI(ModelBinary, *Function);
    if (Callable(Result) != !ShouldFail)
      return false;
  }

  return true;
}

template<typename ABI>
static bool testSuccess(const char *Source, CallableType C = DefaultCallable) {
  return testImpl<ABI, false>(Source, C);
}

template<typename ABI>
static bool testFailure(const char *Source, CallableType C = DefaultCallable) {
  return testImpl<ABI, true>(Source, C);
}

BOOST_AUTO_TEST_CASE(throwaway) {
  using ABI = abi::ABI<model::abi::Microsoft_x64>;
  auto Success = ABI_TEST_DATA::success::Microsoft_x64;
  auto Failure = ABI_TEST_DATA::failure::Microsoft_x64;
  // TODO: Design a robust ABI selection/iteration system.

  BOOST_CHECK(testSuccess<ABI>(Success));
  BOOST_CHECK(testFailure<ABI>(Failure));
}
