//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#define BOOST_TEST_MODULE location
bool init_unit_test();
#include "boost/test/unit_test.hpp"

#include "revng/Model/Binary.h"
#include "revng/Pipes/Location/Instruction.h"
#include "revng/Pipes/Location/TypeField.h"

namespace location = revng::pipes::location;

BOOST_AUTO_TEST_SUITE(RevngLocationInfrastructure);

BOOST_AUTO_TEST_CASE(MetaAddressAsTheKey) {
  location::Instruction Instruction(MetaAddress::fromString("0x123:Generic64"),
                                    MetaAddress::fromString("0x456:Generic64"),
                                    MetaAddress::fromString("0x789:Generic64"));
  revng_check(Instruction.BasicBlock.address() == 0x456);

  Instruction.BasicBlock += Instruction.Address.address();
  revng_check(Instruction.BasicBlock.address() == 0xbdf);

  constexpr auto Expected = "/instruction/0x123:Generic64/"
                            "0xbdf:Generic64/0x789:Generic64";
  // revng_check(Expected == Instruction.toString());
}

BOOST_AUTO_TEST_CASE(Conversions) {
  location::Function Function(MetaAddress::fromString("0x135:Generic64"));

  location::Instruction Instruction{ Function };

  revng_check(Instruction.Function.address() == 0x135);
  Instruction.BasicBlock = MetaAddress::fromString("0x14:Generic64");

  location::BasicBlock Block{ Instruction };
  revng_check(Instruction.BasicBlock.address() == 0x14);

  revng_check(Function.Entry == Block.Function);
}

static model::TypePath makeFunction(model::Binary &Model) {
  model::CABIFunctionType Function;
  Function.CustomName = "my_cool_func";
  Function.OriginalName = "Function_at_0x40012f:Code_x86_64";
  Function.ABI = model::ABI::SystemV_x86_64;

  using UT = model::UpcastableType;
  auto Ptr = UT::make<model::CABIFunctionType>(std::move(Function));

  return Model.recordNewType(std::move(Ptr));
}

BOOST_AUTO_TEST_CASE(TypeIDAsTheKey) {
  TupleTree<model::Binary> NewModel;
  auto FunctionPath = makeFunction(*NewModel);

  location::TypeField Field(FunctionPath.get()->key(), 4);

  auto FetchedPath = NewModel->getTypePath(Field.TypeKey);
  revng_check(FetchedPath.getConst()->name() == "my_cool_func");

  // std::string ID = FunctionPath.toString();
  // ID = ID.substr(ID.find_last_of('/') + 1);
  // revng_check(Field.toString() == "/type-field/" + ID + "/4");
}

BOOST_AUTO_TEST_SUITE_END();
