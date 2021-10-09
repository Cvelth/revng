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

#include "helpers/ABI_TEST_x64.h"

template<DerivesFrom<model::Type> DerivedType>
static std::vector<const DerivedType *>
chooseTypeImpl(const SortedVector<UpcastablePointer<model::Type>> &Types) {
  std::vector<const DerivedType *> Result;
  for (const auto &Type : Types)
    if (auto *Upscaled = llvm::dyn_cast<DerivedType>(Type.get()))
      Result.emplace_back(Upscaled);
  return Result;
}

static std::vector<const model::RawFunctionType *>
chooseRawFunctions(const SortedVector<UpcastablePointer<model::Type>> &Types) {
  return chooseTypeImpl<model::RawFunctionType>(Types);
}

static std::vector<const model::CABIFunctionType *>
chooseCABIFunctions(const SortedVector<UpcastablePointer<model::Type>> &Types) {
  return chooseTypeImpl<model::CABIFunctionType>(Types);
}

template<model::abi::Values ABI>
bool testImpl(std::string_view Input, std::string_view Output) {
  auto Deserialized = TupleTree<model::Binary>::deserialize(Input);
  BOOST_REQUIRE(Deserialized);
  auto &ModelBinary = **Deserialized;
  BOOST_REQUIRE(ModelBinary.verify());
  BOOST_REQUIRE(ModelBinary.Architecture == model::abi::getArchitecture(ABI));

  auto RawFunctions = chooseRawFunctions(ModelBinary.Types);
  std::cout << "Raw function count: " << RawFunctions.size() << std::endl;

  TupleTree<model::Binary> OutputBinary;
  for (auto *Function : RawFunctions) {
    auto Result = abi::ABI<ABI>::toCABI(*OutputBinary, *Function);
    if (Result.has_value()) {
      auto P = model::UpcastableType::make<model::CABIFunctionType>(*Result);
      OutputBinary->recordNewType(std::move(P));
      std::cout << "Adding.\n";
    } else {
      std::cout << "Ignoring.\n";
    }
  }

  auto CABIFunctions = chooseCABIFunctions(OutputBinary->Types);
  std::cout << "CABI function count: " << CABIFunctions.size() << std::endl;

  std::string Serialized;
  OutputBinary.serialize(Serialized);

  std::cout << "\n\n" << Serialized << "\n" << std::endl;

  return true;
}

BOOST_AUTO_TEST_CASE(throwaway) {
  using namespace model::abi;
  testImpl<SystemV_x86_64>(ABI_TEST::Input_x86_64,
                           ABI_TEST::SystemV_x86_64,
                           ABI_TEST::SystemV_x86_64_IDs);
  testImpl<Microsoft_x64>(ABI_TEST::Input_x86_64,
                          ABI_TEST::Microsoft_x64,
                          ABI_TEST::Microsoft_x64_IDs);
  testImpl<Microsoft_x64_vectorcall>(ABI_TEST::Input_x86_64,
                                     ABI_TEST::Microsoft_x64_vectorcall,
                                     ABI_TEST::Microsoft_x64_vectorcall_IDs);
  testImpl<Microsoft_x64_clrcall>(ABI_TEST::Input_x86_64,
                                  ABI_TEST::Microsoft_x64_clrcall,
                                  ABI_TEST::Microsoft_x64_clrcall_IDs);
}
