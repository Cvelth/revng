/// \file Convert.cpp
/// \brief

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Model/Processing.h"
#include "revng/StackAnalysis/ABI.h"

#include "Convert.h"

template<DerivesFrom<model::Type> DerivedType>
static std::vector<DerivedType *>
chooseTypes(SortedVector<UpcastablePointer<model::Type>> &Types) {
  std::vector<DerivedType *> Result;
  for (model::UpcastableType &Type : Types)
    if (auto *Upscaled = llvm::dyn_cast<DerivedType>(Type.get()))
      Result.emplace_back(Upscaled);
  return Result;
}

static auto removeFunctions(TupleTree<model::Binary> &Binary) {
  std::set<const model::Type *> Functions;
  for (model::UpcastableType &Type : Binary->Types)
    if (Type->Kind == model::TypeKind::RawFunctionType
        || Type->Kind == model::TypeKind::CABIFunctionType)
      Functions.emplace(Type.get());

  return model::dropTypesDependingOnTypes(Binary, Functions);
}

static std::optional<model::UpcastableType>
tryConvertToCABI(const model::RawFunctionType *Function,
                 model::abi::Values ABI,
                 TupleTree<model::Binary> &Binary) {
  auto Result = abi::convertToCABI(ABI, *Binary, *Function);
  if (Result.has_value()) {
    Result->ID = Function->ID;
    auto *ReturnValueType = Result->ReturnType.UnqualifiedType.get();
    auto TypeIterator = Binary->Types.find(ReturnValueType->key());
    revng_assert(TypeIterator != Binary->Types.end());
    if (ReturnValueType->Kind == model::TypeKind::Struct)
      TypeIterator->get()->ID = 4000000000 + Function->ID;
    model::TypePath TypePath = Binary->getTypePath(TypeIterator->get());
    Result->ReturnType.UnqualifiedType = TypePath;

    return model::UpcastableType::make<model::CABIFunctionType>(*Result);
  }

  return std::nullopt;
}

static std::optional<model::UpcastableType>
tryConvertToRaw(const model::CABIFunctionType *Function,
                model::abi::Values ABI,
                TupleTree<model::Binary> &Binary) {
  auto Result = abi::convertToRaw(ABI, *Binary, *Function);
  if (Result.has_value()) {
    Result->ID = Function->ID;
    return model::UpcastableType::make<model::RawFunctionType>(*Result);
  }

  return std::nullopt;
}

int convertToCABI(TupleTree<model::Binary> &Binary,
                  model::abi::Values ABI,
                  llvm::raw_fd_ostream &OutputStream) {
  auto Architecture = model::abi::getArchitecture(ABI);
  if (Architecture != Binary->Architecture) {
    llvm::errs() << "Input model architecture is not supported by the ABI (on "
                 << model::abi::getName(ABI).data() << ")\n";
    return 5;
  }

  auto RawFunctions = chooseTypes<model::RawFunctionType>(Binary->Types);
  auto CABIFunctions = chooseTypes<model::CABIFunctionType>(Binary->Types);
  for (auto *Function : CABIFunctions)
    Function->ABI = ABI;
  if (!Binary.verify()) {
    llvm::errs() << "Input model verification has failed (on "
                 << model::abi::getName(ABI).data() << ")\n";
    return 6;
  }

  TupleTree<model::Binary> Result = Binary.clone({});
  if (!Result->verify()) {
    std::string Serialized;
    Result.serialize(Serialized);
    llvm::errs() << "Result model verification failed (on "
                 << model::abi::getName(ABI).data() << "):\n"
                 << Serialized;
    return 7;
  }

  auto RemovedFunctionCount = removeFunctions(Result);
  if (RemovedFunctionCount != RawFunctions.size() + CABIFunctions.size()) {
    llvm::errs() << "Function count is inconsistent, it should be "
                 << (RawFunctions.size() + CABIFunctions.size()) << " but "
                 << RemovedFunctionCount << " was found instead.\n";
    return 8;
  }

  for (model::RawFunctionType *Function : RawFunctions)
    if (auto P = tryConvertToCABI(Function, ABI, Result))
      Result->recordNewType(std::move(P.value()));

  std::string Serialized;
  Result.serialize(Serialized);
  if (!Result->verify()) {
    llvm::errs() << "Result model verification failed (on "
                 << model::abi::getName(ABI).data() << "):\n"
                 << Serialized;
    return 9;
  }

  OutputStream << Serialized;
  return 0;
}

int convertToRaw(TupleTree<model::Binary> &Binary,
                 model::abi::Values ABI,
                 llvm::raw_fd_ostream &OutputStream) {
  auto Architecture = model::abi::getArchitecture(ABI);
  if (Architecture != Binary->Architecture) {
    llvm::errs() << "Input model architecture is not supported by the ABI ("
                 << model::abi::getName(ABI).data() << ")\n";
    return 33;
  }

  auto RawFunctions = chooseTypes<model::RawFunctionType>(Binary->Types);
  auto CABIFunctions = chooseTypes<model::CABIFunctionType>(Binary->Types);
  for (auto *Function : CABIFunctions)
    Function->ABI = ABI;
  if (!Binary.verify()) {
    llvm::errs() << "Input model verification has failed (on "
                 << model::abi::getName(ABI).data() << ")\n";
    return 34;
  }

  TupleTree<model::Binary> Result = Binary.clone({});
  if (!Result->verify()) {
    std::string Serialized;
    Result.serialize(Serialized);
    llvm::errs() << "Result model verification failed (on "
                 << model::abi::getName(ABI).data() << "):\n"
                 << Serialized;
    return 35;
  }

  auto RemovedFunctionCount = removeFunctions(Result);
  if (RemovedFunctionCount != RawFunctions.size() + CABIFunctions.size()) {
    llvm::errs() << "Function count is inconsistent, it should be "
                 << (RawFunctions.size() + CABIFunctions.size()) << " but "
                 << RemovedFunctionCount << " was found instead.\n";
    return 36;
  }

  for (model::CABIFunctionType *Function : CABIFunctions)
    if (auto P = tryConvertToRaw(Function, ABI, Result))
      Result->recordNewType(std::move(P.value()));

  std::string Serialized;
  Result.serialize(Serialized);
  if (!Result->verify()) {
    llvm::errs() << "Result model verification failed (on "
                 << model::abi::getName(ABI).data() << "):\n"
                 << Serialized;
    return 37;
  }

  OutputStream << Serialized;
  return 0;
}
