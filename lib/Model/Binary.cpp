/// \file Binary.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/Regex.h"

#include "revng/Model/Binary.h"
#include "revng/Model/TypeSystemPrinter.h"

namespace {

Logger<> FieldAccessedLogger("field-accessed");

constexpr const char *StructNameHelpText = "regex that will make the program "
                                           "assert when a model struct which "
                                           "name matches this option is "
                                           "accessed. NOTE: enable "
                                           "field-accessed logger, optionally "
                                           "break on onFieldAccess from gdb.";
llvm::cl::opt<std::string> StructNameRegex("tracking-debug-struct-name",
                                           llvm::cl::desc(StructNameHelpText),
                                           llvm::cl::init(""),
                                           llvm::cl::cat(MainCategory));
constexpr const char *FieldNameHelpText = "regex that will "
                                          "make the "
                                          "program assert when "
                                          "a field "
                                          "of a model struct "
                                          "which name "
                                          "matches this "
                                          "option accessed. NOTE: enable "
                                          "field-accessed logger, optionally "
                                          "break on onFieldAccess from gdb.";

llvm::cl::opt<std::string> FieldNameRegex("tracking-debug-field-name",
                                          llvm::cl::desc(FieldNameHelpText),
                                          llvm::cl::init(""),
                                          llvm::cl::cat(MainCategory));

/// This is disabled by default, so it's fine to use something like this
/// internally to make debugging easier.
void onFieldAccess(llvm::StringRef FieldName,
                   llvm::StringRef StructName) debug_function;
void onFieldAccess(llvm::StringRef FieldName, llvm::StringRef StructName) {
  FieldAccessedLogger << ((llvm::StringRef("Field ") + FieldName + " of struct "
                           + StructName + " accessed")
                            .str()
                            .c_str());
  FieldAccessedLogger.flush();
}

} // namespace

void fieldAccessed(llvm::StringRef FieldName, llvm::StringRef StructName) {
  if (StructNameRegex == "" and FieldNameRegex == "")
    return;

  llvm::Regex Reg(StructNameRegex);
  if (StructNameRegex != "" and not Reg.match(StructName))
    return;

  llvm::Regex Reg2(FieldNameRegex);
  if (FieldNameRegex != "" and not Reg2.match(FieldName))
    return;

  onFieldAccess(FieldName, StructName);
}

std::pair<model::TypeDefinition &, model::UpcastableType>
model::Binary::recordNewType(model::UpcastableTypeDefinition &&T) {
  revng_assert(!T.empty());

  // Assign progressive ID
  if (T->ID() != 0) {
    std::string Error = "Newly recorded types must not have an ID.\n"
                        + serializeToString(*T);
    revng_abort(Error.c_str());
  }

  T->ID() = getAvailableTypeID();

  auto [It, Success] = TypeDefinitions().insert(T);
  revng_assert(Success);

  return { **It, makeType((*It)->key()) };
}

uint64_t model::Binary::getAvailableTypeID() const {
  if (TypeDefinitions().empty())
    return 0;

  return TypeDefinitions().rbegin()->get()->ID() + 1;
}

static std::string toIdentifier(const MetaAddress &Address) {
  return model::Identifier::sanitize(Address.toString()).str().str();
}

model::Identifier model::Function::name() const {
  using llvm::Twine;
  if (not CustomName().empty()) {
    return CustomName();
  } else {
    auto AutomaticName = (Twine("_function_") + toIdentifier(Entry())).str();
    return Identifier(AutomaticName);
  }
}

model::Identifier model::DynamicFunction::name() const {
  using llvm::Twine;
  if (not CustomName().empty()) {
    return CustomName();
  } else {
    auto AutomaticName = (Twine("_dynamic_") + OriginalName()).str();
    return Identifier(AutomaticName);
  }
}

model::Identifier model::Segment::name() const {
  using llvm::Twine;
  if (not CustomName().empty()) {
    return CustomName();
  } else {
    auto AutomaticName = (Twine("_segment_") + toIdentifier(StartAddress())
                          + "_" + Twine(VirtualSize()))
                           .str();
    return Identifier(AutomaticName);
  }
}

namespace model::RelocationType {

Values fromELFRelocation(model::Architecture::Values Architecture,
                         unsigned char ELFRelocation) {
  using namespace llvm::ELF;
  switch (Architecture) {
  case model::Architecture::x86:
    switch (ELFRelocation) {
    case R_386_RELATIVE:
    case R_386_32:
      return AddAbsoluteAddress32;

    case R_386_JUMP_SLOT:
    case R_386_GLOB_DAT:
      return WriteAbsoluteAddress32;

    case R_386_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  case model::Architecture::x86_64:
    switch (ELFRelocation) {
    case R_X86_64_RELATIVE:
      return AddAbsoluteAddress64;

    case R_X86_64_JUMP_SLOT:
    case R_X86_64_GLOB_DAT:
    case R_X86_64_64:
      return WriteAbsoluteAddress64;

    case R_X86_64_32:
      return WriteAbsoluteAddress32;

    case R_X86_64_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  case model::Architecture::arm:
    switch (ELFRelocation) {
    case R_ARM_RELATIVE:
      return AddAbsoluteAddress32;

    case R_ARM_JUMP_SLOT:
    case R_ARM_GLOB_DAT:
      return WriteAbsoluteAddress32;

    case R_ARM_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  case model::Architecture::aarch64:
    return Invalid;

  case model::Architecture::mips:
  case model::Architecture::mipsel:
    switch (ELFRelocation) {
    case R_MIPS_IMPLICIT_RELATIVE:
      return AddAbsoluteAddress32;

    case R_MIPS_JUMP_SLOT:
    case R_MIPS_GLOB_DAT:
      return WriteAbsoluteAddress32;

    case R_MIPS_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  case model::Architecture::systemz:
    switch (ELFRelocation) {
    case R_390_GLOB_DAT:
      return WriteAbsoluteAddress64;

    case R_390_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  default:
    revng_abort();
  }
}

bool isELFRelocationBaseRelative(model::Architecture::Values Architecture,
                                 unsigned char ELFRelocation) {
  using namespace llvm::ELF;
  switch (Architecture) {
  case model::Architecture::x86:
    switch (ELFRelocation) {
    case R_386_RELATIVE:
      return true;

    case R_386_32:
    case R_386_JUMP_SLOT:
    case R_386_GLOB_DAT:
      return false;

    case R_386_COPY:
      // TODO: use

    default:
      return Invalid;
    }

  case model::Architecture::x86_64:
    switch (ELFRelocation) {
    case R_X86_64_RELATIVE:
      return true;

    case R_X86_64_JUMP_SLOT:
    case R_X86_64_GLOB_DAT:
    case R_X86_64_64:
    case R_X86_64_32:
      return false;

    case R_X86_64_COPY:
      // TODO: use

    default:
      return Invalid;
    }

  case model::Architecture::arm:
    switch (ELFRelocation) {
    case R_ARM_RELATIVE:
      return true;

    case R_ARM_JUMP_SLOT:
    case R_ARM_GLOB_DAT:
      return false;

    case R_ARM_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  case model::Architecture::aarch64:
    return Invalid;

  case model::Architecture::mips:
  case model::Architecture::mipsel:
    switch (ELFRelocation) {
    case R_MIPS_IMPLICIT_RELATIVE:
      return true;

    case R_MIPS_JUMP_SLOT:
    case R_MIPS_GLOB_DAT:
      return false;

    case R_MIPS_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  case model::Architecture::systemz:
    switch (ELFRelocation) {
    case R_390_GLOB_DAT:
      return false;

    case R_390_COPY:
      // TODO: use
    default:
      return Invalid;
    }

  default:
    revng_abort();
  }
}

Values formCOFFRelocation(model::Architecture::Values Architecture) {
  switch (Architecture) {
  case model::Architecture::x86:
  case model::Architecture::arm:
  case model::Architecture::mips:
  case model::Architecture::mipsel:
    return WriteAbsoluteAddress32;

  case model::Architecture::x86_64:
  case model::Architecture::aarch64:
  case model::Architecture::systemz:
    return WriteAbsoluteAddress64;
  default:
    revng_abort();
  }
}

} // namespace model::RelocationType

std::string model::Binary::toString() const {
  TrackGuard Guard(*this);
  return serializeToString(*this);
}

void model::Function::dumpTypeGraph(const char *Path) const {
  TrackGuard Guard(*this);
  std::error_code EC;
  llvm::raw_fd_ostream Out(Path, EC);
  if (EC)
    revng_abort(EC.message().c_str());

  TypeSystemPrinter TSPrinter(Out);
  TSPrinter.print(*this);
}

void model::Binary::dumpTypeGraph(const char *Path) const {
  TrackGuard Guard(*this);

  std::error_code EC;
  llvm::raw_fd_ostream Out(Path, EC);
  if (EC)
    revng_abort(EC.message().c_str());

  TypeSystemPrinter TSPrinter(Out);
  TSPrinter.print(*this);
}

//
// OLD
//

/*
#include <system_error>

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/DOTGraphTraits.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/ADT/GenericGraph.h"
#include "revng/Model/Binary.h"
#include "revng/Model/VerifyHelper.h"
#include "revng/Support/OverflowSafeInt.h"
#include "revng/Support/YAMLTraits.h"
#include "revng/TupleTree/Tracking.h"

} // namespace model
*/
