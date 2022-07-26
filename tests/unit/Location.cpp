//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#define BOOST_TEST_MODULE location
bool init_unit_test();
#include "boost/test/unit_test.hpp"

#include "revng/Model/Binary.h"
#include "revng/Pipes/Ranks.h"

BOOST_AUTO_TEST_SUITE(RevngLocationInfrastructure);

BOOST_AUTO_TEST_CASE(DefinitionStyles) {
  const MetaAddress A0 = MetaAddress::fromString("0x1:Generic64");
  const MetaAddress A1 = MetaAddress::fromString("0x2:Generic64");
  const MetaAddress A2 = MetaAddress::fromString("0x3:Generic64");
  auto Location = revng::pipes::rank::Instruction.makeLocation(A0, A1, A2);

  constexpr auto S = "/instruction/0x1:Generic64/0x2:Generic64/0x3:Generic64";
  auto MaybeInstruction = revng::pipes::rank::Instruction.locationFromString(S);
  revng_check(MaybeInstruction.has_value());

  revng_check(Location.tuple() == MaybeInstruction->tuple());
  revng_check(Location.toString() == S);
}

BOOST_AUTO_TEST_CASE(MetaAddressAsTheKey) {
  namespace rank = revng::pipes::rank;

  const MetaAddress A0 = MetaAddress::fromString("0x123:Generic64");
  const MetaAddress A1 = MetaAddress::fromString("0x456:Generic64");
  const MetaAddress A2 = MetaAddress::fromString("0x789:Generic64");
  auto Location = rank::Instruction.makeLocation(A0, A1, A2);

  revng_check(Location.get(rank::Function).address() == 0x123);
  revng_check(Location.get(rank::BasicBlock).address() == 0x456);
  revng_check(Location.get(rank::Instruction).address() == 0x789);

  Location.get(rank::BasicBlock) += Location.get(rank::Instruction).address();
  revng_check(Location.get(rank::BasicBlock).address() == 0xbdf);

  constexpr auto Expected = "/instruction/0x123:Generic64/"
                            "0xbdf:Generic64/0x789:Generic64";
  revng_check(Location.toString() == Expected);
  revng_check(Location.toString() == serializeToString(Location));
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
  auto Function = makeFunction(*NewModel);

  namespace rank = revng::pipes::rank;
  auto FieldLocation = rank::TypeField.makeLocation(Function.get()->key(), 2);

  auto FetchedPath = NewModel->getTypePath(FieldLocation.get(rank::Type));
  revng_check(FetchedPath.getConst()->name() == "my_cool_func");

  std::string ID = Function.toString();
  ID = ID.substr(ID.find_last_of('/') + 1);
  revng_check(FieldLocation.toString() == "/type-field/" + ID + "/2");
  revng_check(FieldLocation.toString() == serializeToString(FieldLocation));
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

  namespace rank = revng::pipes::rank;
  for (auto TestCase : TestCases) {
    bool ParsedOnce = false;

    if (auto Binary = rank::Binary.locationFromString(TestCase)) {
      revng_check(TestCase == TestCases[0]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(TestCase == Binary->toString());
    }

    if (auto Instruction = rank::Instruction.locationFromString(TestCase)) {
      revng_check(TestCase == TestCases[1]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(Instruction->get(rank::Function).address() == 0x12);
      revng_check(Instruction->get(rank::BasicBlock).address() == 0x34);
      revng_check(Instruction->get(rank::Instruction).address() == 0x56);

      revng_check(TestCase == Instruction->toString());
    }

    if (auto Type = rank::Type.locationFromString(TestCase)) {
      revng_check(TestCase == TestCases[2]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(std::get<uint64_t>(Type->get(rank::Type)) == 1026);

      revng_check(TestCase == Type->toString());
    }

    if (auto RawRange = rank::RawByteRange.locationFromString(TestCase)) {
      revng_check(TestCase == TestCases[3]);
      revng_check(ParsedOnce == false);
      ParsedOnce = true;

      revng_check(RawRange->get(rank::RawByte).address() == 0x78);
      revng_check(RawRange->get(rank::RawByteRange).address() == 0x90);

      revng_check(TestCase == RawRange->toString());
    }

    revng_check(!rank::Function.locationFromString(TestCase).has_value());
    revng_check(!rank::BasicBlock.locationFromString(TestCase).has_value());
    revng_check(!rank::TypeField.locationFromString(TestCase).has_value());
    revng_check(!rank::RawByte.locationFromString(TestCase).has_value());

    using namespace std::string_literals;
    revng_check(ParsedOnce,
                ("Parsing of '"s + TestCase.data() + "' failed").c_str());
  }
}

BOOST_AUTO_TEST_SUITE_END();
