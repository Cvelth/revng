//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#define BOOST_TEST_MODULE location
bool init_unit_test();
#include "boost/test/unit_test.hpp"

#include "revng/Model/Binary.h"
#include "revng/Pipes/Locations.h"

namespace location = revng::pipes::location;

BOOST_AUTO_TEST_SUITE(RevngLocationInfrastructure);

BOOST_AUTO_TEST_CASE(MetaAddressAsTheKey) {
  using namespace location;
  auto Instr = Instruction.make(MetaAddress::fromString("0x123:Generic64"),
                                MetaAddress::fromString("0x456:Generic64"),
                                MetaAddress::fromString("0x789:Generic64"));

  revng_check(std::get<"basic-block">(Instr).address() == 0x456);

  std::get<"basic-block">(Instr) += std::get<"address">(Instr).address();

  revng_check(std::get<"basic-block">(Instr).address() == 0xbdf);

  constexpr auto Expected = "/instruction/0x123:Generic64/"
                            "0xbdf:Generic64/0x789:Generic64";
  revng_check(Instr.toString() == Expected);
  revng_check(Instr.toString() == serializeToString(Instr));
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

  auto Field = revng::pipes::location::TypeField.make();
  std::get<"type-key">(Field) = FunctionPath.get()->key();
  std::get<"field-index">(Field) = 2; // And arbitrary field index. WIP

  auto FetchedPath = NewModel->getTypePath(std::get<"type-key">(Field));
  revng_check(FetchedPath.getConst()->name() == "my_cool_func");

  std::string ID = FunctionPath.toString();
  ID = ID.substr(ID.find_last_of('/') + 1);
  revng_check(Field.toString() == "/type-field/" + ID + "/2");
  revng_check(Field.toString() == serializeToString(Field));
}

BOOST_AUTO_TEST_CASE(Serialization) {
  constexpr std::array<std::string_view, 4> TestCases{
    // clang-format off
    "/binary",
    "/instruction/0x12:Generic64/0x34:Generic64/0x56:Generic64",
    "/type/PrimitiveType-1026",
    "/raw-byte-range/0x78:Generic64/0x90:Generic64"
    // clang-format on
  };

  for (auto TestCase : TestCases) {
    bool ParsedOnce = false;

    if (auto Binary = location::Binary.fromString(TestCase)) {
      revng_check(TestCase == TestCases[0]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(TestCase == Binary->toString());
    }

    if (auto Instruction = location::Instruction.fromString(TestCase)) {
      revng_check(TestCase == TestCases[1]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(std::get<"function">(*Instruction).address() == 0x12);
      revng_check(std::get<"basic-block">(*Instruction).address() == 0x34);
      revng_check(std::get<"address">(*Instruction).address() == 0x56);

      revng_check(TestCase == Instruction->toString());
    }

    if (auto Type = location::Type.fromString(TestCase)) {
      revng_check(TestCase == TestCases[2]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(std::get<uint64_t>(std::get<"key">(*Type)) == 1026);

      revng_check(TestCase == Type->toString());
    }

    if (auto RawRange = location::RawByteRange.fromString(TestCase)) {
      revng_check(TestCase == TestCases[3]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(std::get<"from">(*RawRange).address() == 0x78);
      revng_check(std::get<"to">(*RawRange).address() == 0x90);

      revng_check(TestCase == RawRange->toString());
    }

    revng_check(location::Function.fromString(TestCase) == std::nullopt);
    revng_check(location::BasicBlock.fromString(TestCase) == std::nullopt);
    revng_check(location::TypeField.fromString(TestCase) == std::nullopt);
    revng_check(location::RawByte.fromString(TestCase) == std::nullopt);

    using namespace std::string_literals;
    revng_check(ParsedOnce,
                ("Parsing of '"s + TestCase.data() + "' failed").c_str());
  }
}

BOOST_AUTO_TEST_SUITE_END();
