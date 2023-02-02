/// \file ConvertToRawFunctionType.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/ABI/FunctionType/Layout.h"
#include "revng/ABI/FunctionType/Support.h"
#include "revng/Model/Binary.h"
#include "revng/Model/Pass/PurgeUnnamedAndUnreachableTypes.h"
#include "revng/Model/VerifyHelper.h"
#include "revng/Pipeline/Analysis.h"
#include "revng/Pipeline/RegisterAnalysis.h"
#include "revng/Pipes/Kinds.h"
#include "revng/TupleTree/TupleTree.h"

class ConvertToRawFunctionType {
public:
  static constexpr auto Name = "ConvertToRawFunctionType";
  inline static const std::tuple Options = { pipeline::Option("abi",
                                                              "Invalid"s) };
  std::vector<std::vector<pipeline::Kind *>> AcceptedKinds = {};

  void run(pipeline::Context &Context, std::string OverrideABI) {
    auto &Model = revng::getWritableModelFromContext(Context);
    revng_assert(!OverrideABI.empty());

    // Note, if `OverrideABI` was explicitly set,
    // override the one from the function with it.
    std::optional<model::ABI::Values> ABI = model::ABI::fromName(OverrideABI);
    if (ABI.value() == model::ABI::Invalid)
      ABI = std::nullopt;

    model::VerifyHelper VH;

    using abi::FunctionType::filterTypes;
    auto ToConvert = filterTypes<model::CABIFunctionType>(Model->Types());
    for (model::CABIFunctionType *Old : ToConvert) {
      auto New = abi::FunctionType::convertToRaw(*Old, Model, ABI);

      // Make sure the returned type is valid,
      revng_assert(New.isValid());

      // and verifies.
      revng_assert(New.get()->verify(VH));
    }

    // Don't forget to clean up any possible remainders of removed types.
    purgeUnnamedAndUnreachableTypes(Model);
  }
};

static pipeline::RegisterAnalysis<ConvertToRawFunctionType> ToRawAnalysis;
