/// \file ABI.cpp
/// \brief ABI tests

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#define BOOST_TEST_MODULE ABI
bool init_unit_test();
#include "boost/test/unit_test.hpp"

#include "revng/Model/Binary.h"
#include "revng/Model/TupleTreeDiff.h"
#include "revng/StackAnalysis/ABI.h"

#include "helpers/abi_test_x64.h"
#include "helpers/abi_test_x86.h"

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
bool testImpl(std::string_view Input,
              std::string_view Output,
              const SortedVector<size_t> &SuccessfulIDs) {
  auto Deserialized = TupleTree<model::Binary>::deserialize(Input);
  BOOST_REQUIRE_MESSAGE(Deserialized,
                        "Fail to deserialize the input on "
                          << model::abi::getName(ABI).data());
  auto &ModelBinary = **Deserialized;
  BOOST_REQUIRE_MESSAGE(ModelBinary.verify(),
                        "Model verification failed on "
                          << model::abi::getName(ABI).data());

  constexpr auto Architecture = model::abi::getArchitecture(ABI);
  BOOST_REQUIRE_MESSAGE(ModelBinary.Architecture == Architecture,
                        "Deserialized model architecture is not supported by "
                        "the ABI on "
                          << model::abi::getName(ABI).data());

  auto RawFunctions = chooseRawFunctions(ModelBinary.Types);
  BOOST_TEST_LAZY_MSG("Raw function count: " << RawFunctions.size());

  TupleTree<model::Binary> OutputBinary;
  OutputBinary->Architecture = ModelBinary.Architecture;
  for (auto *Function : RawFunctions) {
    auto Iterator = SuccessfulIDs.find(Function->ID);
    auto Result = abi::ABI<ABI>::toCABI(*OutputBinary, *Function);
    if (Result.has_value()) {
      Result->ID = Function->ID;
      auto *ReturnValueType = Result->ReturnType.UnqualifiedType.get();
      if (ReturnValueType->Kind == model::TypeKind::Struct)
        ReturnValueType->ID = 8000000000 + Function->ID;
      auto NewReturnTypePath = OutputBinary->getTypePath(ReturnValueType);
      Result->ReturnType.UnqualifiedType = NewReturnTypePath;

      auto P = model::UpcastableType::make<model::CABIFunctionType>(*Result);
      OutputBinary->recordNewType(std::move(P));
      BOOST_CHECK_MESSAGE(Iterator != SuccessfulIDs.end(),
                          "Converting a function (with ID="
                            << Function->ID
                            << ") succeeded (it should have failed) on "
                            << model::abi::getName(ABI).data());
    } else {
      BOOST_CHECK_MESSAGE(Iterator == SuccessfulIDs.end(),
                          "Converting a function (with ID="
                            << Function->ID
                            << ") failed (it should have succeeded) on "
                            << model::abi::getName(ABI).data());
    }
  }

  auto CABIFunctions = chooseCABIFunctions(OutputBinary->Types);
  BOOST_TEST_LAZY_MSG("CABI function count: " << RawFunctions.size());

  std::string Serialized;
  OutputBinary.serialize(Serialized);

  auto DeserializedOutput = TupleTree<model::Binary>::deserialize(Output);
  BOOST_REQUIRE_MESSAGE(DeserializedOutput,
                        "Fail to deserialize the output on "
                          << model::abi::getName(ABI).data());
  auto &ExpectedBinary = **DeserializedOutput;
  BOOST_REQUIRE_MESSAGE(ExpectedBinary.verify(),
                        "Expected model verification failed on "
                          << model::abi::getName(ABI).data());

  BOOST_REQUIRE_MESSAGE(ExpectedBinary.Architecture == Architecture,
                        "Expected model architecture is not supported by "
                        "the ABI on "
                          << model::abi::getName(ABI).data());

  BOOST_CHECK_MESSAGE(diff(*OutputBinary, ExpectedBinary).Changes.empty(),
                      "Received output is different from what was expected on "
                        << model::abi::getName(ABI).data() << ":\n"
                        << Serialized << "\n\nExpected:\n"
                        << Output);

  return true;
}

BOOST_AUTO_TEST_CASE(x64_ABIs) {
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

BOOST_AUTO_TEST_CASE(x86_ABIs) {
  using namespace model::abi;
  testImpl<SystemV_x86>(ABI_TEST::Input_x86,
                        ABI_TEST::SystemV_x86,
                        ABI_TEST::SystemV_x86_IDs);
  testImpl<SystemV_x86_regparm_1>(ABI_TEST::Input_x86,
                                  ABI_TEST::SystemV_x86_regparm_1,
                                  ABI_TEST::SystemV_x86_regparm_1_IDs);
  testImpl<SystemV_x86_regparm_2>(ABI_TEST::Input_x86,
                                  ABI_TEST::SystemV_x86_regparm_2,
                                  ABI_TEST::SystemV_x86_regparm_2_IDs);
  testImpl<SystemV_x86_regparm_3>(ABI_TEST::Input_x86,
                                  ABI_TEST::SystemV_x86_regparm_3,
                                  ABI_TEST::SystemV_x86_regparm_3_IDs);
  testImpl<Microsoft_x86_cdecl>(ABI_TEST::Input_x86,
                                ABI_TEST::Microsoft_x86_cdecl,
                                ABI_TEST::Microsoft_x86_cdecl_IDs);
  testImpl<Microsoft_x86_stdcall>(ABI_TEST::Input_x86,
                                  ABI_TEST::Microsoft_x86_stdcall,
                                  ABI_TEST::Microsoft_x86_stdcall_IDs);
  testImpl<Microsoft_x86_thiscall>(ABI_TEST::Input_x86,
                                   ABI_TEST::Microsoft_x86_thiscall,
                                   ABI_TEST::Microsoft_x86_thiscall_IDs);
  testImpl<Microsoft_x86_fastcall>(ABI_TEST::Input_x86,
                                   ABI_TEST::Microsoft_x86_fastcall,
                                   ABI_TEST::Microsoft_x86_fastcall_IDs);
  testImpl<Microsoft_x86_clrcall>(ABI_TEST::Input_x86,
                                  ABI_TEST::Microsoft_x86_clrcall,
                                  ABI_TEST::Microsoft_x86_clrcall_IDs);
  testImpl<Microsoft_x86_vectorcall>(ABI_TEST::Input_x86,
                                     ABI_TEST::Microsoft_x86_vectorcall,
                                     ABI_TEST::Microsoft_x86_vectorcall_IDs);
}
