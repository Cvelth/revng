//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#define BOOST_TEST_MODULE location
bool init_unit_test();
#include "boost/test/unit_test.hpp"

#include "revng/Location/Location.h"
#include "revng/Model/Binary.h"

using namespace revng;
using namespace revng::location;

BOOST_AUTO_TEST_SUITE(RevngLocationInfrastructure);

BOOST_AUTO_TEST_CASE(MetaAddressAsTheKey) {
  Instruction Location;
  Location.Function::Value = MetaAddress::fromString("0x123:Generic64");
  Location.BasicBlock::Value = MetaAddress::fromString("0x456:Generic64");
  Location.Instruction::Value = MetaAddress::fromString("0x789:Generic64");

  revng_check(Location.BasicBlock::Value.address() == 0x456);

  Location.BasicBlock::Value += Location.Instruction::Value.address();

  revng_check(Location.BasicBlock::Value.address() == 0xbdf);

  constexpr auto Expected = "/Instruction/0x123:Generic64/"
                            "0xbdf:Generic64/0x789:Generic64";
  revng_check(Location.toString() == Expected);
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

BOOST_AUTO_TEST_CASE(Conversions) {
  Function F;
  F.Function::Value = MetaAddress::fromString("0x135:Generic64");

  // Instruction I = F; // Fails by design
  // We should avoid default initialization where possible.

  Instruction I;
  static_cast<Function &>(I) = F; // Hack for the sake of this test.

  revng_check(I.Function::Value.address() == 0x135);
  I.BasicBlock::Value = MetaAddress::fromString("0x14:Generic64");

  BasicBlock B = I;
  revng_check(I.BasicBlock::Value.address() == 0x14);

  revng_check(F.Function::Value == B.Function::Value);
}

BOOST_AUTO_TEST_CASE(TypeIDAsTheKey) {
  TupleTree<model::Binary> NewModel;
  auto FunctionPath = makeFunction(*NewModel);

  TypeField Location;
  Location.Type::Value = FunctionPath.get()->key();
  Location.TypeField::Value = 2; // And arbitrary field index. WIP

  auto Name = NewModel->getTypePath(Location.Type::Value).getConst()->name();
  revng_check(Name == "my_cool_func");

  std::string ID = FunctionPath.toString();
  ID = ID.substr(ID.find_last_of('/') + 1);
  revng_check(Location.toString() == "/TypeField/" + ID + "/2");
}

BOOST_AUTO_TEST_CASE(Serialization) {
  constexpr std::array<std::string_view, 4> TestCases{
    // clang-format off
    "/Binary",
    "/Instruction/0x12:Generic64/0x34:Generic64/0x56:Generic64",
    "/Type/PrimitiveType-1026",
    "/RawByteRange/0x78:Generic64/0x90:Generic64"
    // clang-format on
  };

  for (auto TestCase : TestCases) {
    bool ParsedOnce = false;

    auto Binary = location::Binary::fromString(TestCase);
    if (Binary) {
      revng_check(TestCase == TestCases[0]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(TestCase == Binary->toString());
    }

    auto MaybeInstruction = location::Instruction::fromString(TestCase);
    if (MaybeInstruction) {
      revng_check(TestCase == TestCases[1]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(MaybeInstruction->Function::Value.address() == 0x12);
      revng_check(MaybeInstruction->BasicBlock::Value.address() == 0x34);
      revng_check(MaybeInstruction->Instruction::Value.address() == 0x56);

      revng_check(TestCase == MaybeInstruction->toString());
    }

    auto MaybeType = location::Type::fromString(TestCase);
    if (MaybeType) {
      revng_check(TestCase == TestCases[2]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(std::get<uint64_t>(MaybeType->Type::Value) == 1026);

      revng_check(TestCase == MaybeType->toString());
    }

    auto MaybeRawByteRange = location::RawByteRange::fromString(TestCase);
    if (MaybeRawByteRange) {
      revng_check(TestCase == TestCases[3]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(MaybeRawByteRange->FromByte::Value.address() == 0x78);
      revng_check(MaybeRawByteRange->ToByte::Value.address() == 0x90);

      revng_check(TestCase == MaybeRawByteRange->toString());
    }

    revng_check(location::Function::fromString(TestCase) == std::nullopt);
    revng_check(location::BasicBlock::fromString(TestCase) == std::nullopt);
    revng_check(location::TypeField::fromString(TestCase) == std::nullopt);
    revng_check(location::RawByte::fromString(TestCase) == std::nullopt);

    using namespace std::string_literals;
    revng_check(ParsedOnce,
                ("Parsing of '"s + TestCase.data() + "' failed").c_str());
  }
}

BOOST_AUTO_TEST_SUITE_END();
