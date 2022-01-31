/// \file Binary.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Support/DOTGraphTraits.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_os_ostream.h"

#include "revng/ADT/GenericGraph.h"
#include "revng/Model/Binary.h"
#include "revng/Model/VerifyHelper.h"

using namespace llvm;

namespace model {

model::TypePath
Binary::getPrimitiveType(PrimitiveTypeKind::Values V, uint8_t ByteSize) {
  PrimitiveType Temporary(V, ByteSize);
  Type::Key PrimitiveKey{ TypeKind::Primitive, Temporary.ID };
  auto It = Types.find(PrimitiveKey);

  // If we couldn't find it, create it
  if (It == Types.end()) {
    auto *NewPrimitiveType = new PrimitiveType(V, ByteSize);
    It = Types.insert(UpcastablePointer<model::Type>(NewPrimitiveType)).first;
  }

  return getTypePath(It->get());
}

model::TypePath
Binary::getPrimitiveType(PrimitiveTypeKind::Values V, uint8_t ByteSize) const {
  PrimitiveType Temporary(V, ByteSize);
  Type::Key PrimitiveKey{ TypeKind::Primitive, Temporary.ID };
  return getTypePath(Types.at(PrimitiveKey).get());
}

TypePath Binary::recordNewType(UpcastablePointer<Type> &&T) {
  auto It = Types.insert(T).first;
  return getTypePath(It->get());
}

bool Binary::verifyTypes() const {
  return verifyTypes(false);
}

bool Binary::verifyTypes(bool Assert) const {
  VerifyHelper VH(Assert);
  return verifyTypes(VH);
}

bool Binary::verifyTypes(VerifyHelper &VH) const {
  // All types on their own should verify
  std::set<Identifier> Names;
  for (auto &Type : Types) {
    // Verify the type
    if (not Type.get()->verify(VH))
      return VH.fail();

    // Ensure the names are unique
    auto Name = Type->name();
    if (not Names.insert(Name).second)
      return VH.fail(Twine("Multiple types with the following name: ") + Name);
  }

  return true;
}

void Binary::dump() const {
  serialize(dbg, *this);
}

std::string Binary::toString() const {
  std::string S;
  llvm::raw_string_ostream OS(S);
  serialize(OS, *this);
  return S;
}

bool Binary::verify() const {
  return verify(false);
}

bool Binary::verify(bool Assert) const {
  VerifyHelper VH(Assert);
  return verify(VH);
}

bool Binary::verify(VerifyHelper &VH) const {
  // Prepare for checking symbol names. We will populate and check this against
  // functions, dynamic functions, types and enum entries
  std::set<Identifier> Symbols;
  auto CheckCustomName = [&VH, &Symbols, this](const Identifier &CustomName) {
    if (CustomName.empty())
      return true;

    return VH.maybeFail(Symbols.insert(CustomName).second,
                        "Duplicate name: " + CustomName.str().str(),
                        *this);
  };

  for (const Function &F : Functions) {
    // Verify individual functions
    if (not F.verify(VH))
      return VH.fail();

    if (not CheckCustomName(F.CustomName))
      return VH.fail("Duplicate name", F);
  }

  // Verify DynamicFunctions
  for (const DynamicFunction &DF : ImportedDynamicFunctions) {
    if (not DF.verify(VH))
      return VH.fail();

    if (not CheckCustomName(DF.CustomName))
      return VH.fail();
  }

  for (auto &Type : Types) {
    if (not CheckCustomName(Type->CustomName))
      return VH.fail();

    if (auto *Enum = dyn_cast<EnumType>(Type.get()))
      for (auto &Entry : Enum->Entries)
        if (not CheckCustomName(Entry.CustomName))
          return VH.fail();
  }

  //
  // Verify the type system
  //
  return verifyTypes(VH);
}

Identifier Function::name() const {
  using llvm::Twine;
  if (not CustomName.empty()) {
    return CustomName;
  } else {
    // TODO: this prefix needs to be reserved
    auto AutomaticName = (Twine("function_") + Entry.toString()).str();
    return Identifier::fromString(AutomaticName);
  }
}

Identifier DynamicFunction::name() const {
  using llvm::Twine;
  if (not CustomName.empty()) {
    return CustomName;
  } else {
    // TODO: this prefix needs to be reserved
    auto AutomaticName = (Twine("dynamic_function_") + OriginalName).str();
    return Identifier::fromString(AutomaticName);
  }
}

void Function::dump() const {
  serialize(dbg, *this);
}

bool Function::verify() const {
  return verify(false);
}

bool Function::verify(bool Assert) const {
  VerifyHelper VH(Assert);
  return verify(VH);
}

bool Function::verify(VerifyHelper &VH) const {
  if (Prototype.isValid()) {
    // The function has a prototype
    if (not Prototype.get()->verify(VH))
      return VH.fail("Function prototype does not verify", *this);

    const model::Type *FunctionType = Prototype.get();
    if (not(isa<RawFunctionType>(FunctionType)
            or isa<CABIFunctionType>(FunctionType))) {
      return VH.fail("Function prototype is not a RawFunctionType or "
                     "CABIFunctionType",
                     *this);
    }
  }

  for (auto &CallSitePrototype : CallSitePrototypes)
    if (not CallSitePrototype.verify(VH))
      return VH.fail();

  return true;
}

void DynamicFunction::dump() const {
  serialize(dbg, *this);
}

bool DynamicFunction::verify() const {
  return verify(false);
}

bool DynamicFunction::verify(bool Assert) const {
  VerifyHelper VH(Assert);
  return verify(VH);
}

bool DynamicFunction::verify(VerifyHelper &VH) const {
  // Ensure we have a name
  if (OriginalName.size() == 0)
    return VH.fail("Dynamic functions must have a OriginalName", *this);

  // Prototype is present
  if (not Prototype.isValid())
    return VH.fail("Invalid prototype", *this);

  // Prototype is valid
  if (not Prototype.get()->verify(VH))
    return VH.fail();

  const model::Type *FunctionType = Prototype.get();
  if (not(isa<RawFunctionType>(FunctionType)
          or isa<CABIFunctionType>(FunctionType)))
    return VH.fail("The prototype is neither a RawFunctionType nor a "
                   "CABIFunctionType",
                   *this);

  return true;
}

void CallSitePrototype::dump() const {
  serialize(dbg, *this);
}

bool CallSitePrototype::verify() const {
  return verify(false);
}

bool CallSitePrototype::verify(bool Assert) const {
  VerifyHelper VH(Assert);
  return verify(VH);
}

bool CallSitePrototype::verify(VerifyHelper &VH) const {
  // Prototype is present
  if (not Prototype.isValid())
    return VH.fail("Invalid prototype", *this);

  // Prototype is valid
  if (not Prototype.get()->verify(VH))
    return VH.fail();

  return true;
}

} // namespace model
