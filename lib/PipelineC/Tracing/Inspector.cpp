/// \file Runner.cpp
/// \brief Implements the logic for inspecting a trace file. Right now this
///        entails listing and extracting buffers

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/Support/Base64.h"

#include "revng/ADT/ConstexprString.h"
#include "revng/PipelineC/PipelineC.h"
#include "revng/PipelineC/Tracing/Common.h"
#include "revng/PipelineC/Tracing/Trace.h"

static class BufferRegistry {
private:
  llvm::StringMap<std::set<size_t>> Registry;

public:
  BufferRegistry() {
// Add autogenerated registerRunner calls to register the
// other functions automatically
#define FUNCTION(fname) registerFunction<#fname>(fname);
#include "revng/PipelineC/Functions.inc"
#undef FUNCTION
  }

public:
  bool has(const llvm::StringRef Name) { return Registry.count(Name) != 0; }

  std::set<size_t> &operator[](const llvm::StringRef Name) {
    return Registry[Name];
  }

private:
  template<ConstexprString Name, typename ArgTupleT, size_t I = 0>
  void handleArguments() {
    using ArgT = std::tuple_element_t<I, ArgTupleT>;
    constexpr int LH = LengthHint<Name, I>;
    if constexpr (anyOf<ArgT, char *, const char *>() && LH >= 0) {
      std::string StrName(Name);
      Registry[StrName].insert(I);
    }

    if constexpr (I + 1 < std::tuple_size_v<ArgTupleT>)
      handleArguments<Name, ArgTupleT, I + 1>();
  }

  template<ConstexprString Name, typename ReturnT, typename... Args>
  void handleFunction(std::function<ReturnT(Args...)> Function) {
    if constexpr (sizeof...(Args) > 0)
      handleArguments<Name, std::tuple<Args...>>();
  }

  template<ConstexprString Name, typename FunctionT>
  void registerFunction(FunctionT Function) {
    handleFunction<Name>(std::function{ Function });
  }
} BufferRegistry;

inline llvm::Expected<std::vector<char>>
extractBuffer(const revng::tracing::Argument &Arg) {
  revng_assert(Arg.isScalar());
  std::vector<char> Result;
  llvm::Error Err = llvm::decodeBase64(Arg.getScalar(), Result);
  if (Err)
    return std::move(Err);

  llvm::consumeError(std::move(Err));
  return Result;
}

namespace revng::tracing {

std::vector<BufferLocation> Trace::listBuffers() const {
  std::vector<BufferLocation> Result;
  for (size_t CommandI = 0; CommandI < this->Commands.size(); CommandI++) {
    auto &Command = this->Commands[CommandI];
    if (BufferRegistry.has(Command.Name)) {
      for (size_t ArgI : BufferRegistry[Command.Name]) {
        Result.push_back({ Command.Name, CommandI, ArgI });
      }
    }
  }

  return Result;
}

llvm::Expected<std::vector<char>>
Trace::getBuffer(const BufferLocation &Location) const {
  return getBuffer(Location.CommandNumber, Location.ArgumentNumber);
};

llvm::Expected<std::vector<char>>
Trace::getBuffer(size_t CommandNo, size_t ArgNo) const {
  if (CommandNo > this->Commands.size())
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "Command number OOB");

  const revng::tracing::Command &Command = this->Commands[CommandNo];
  if (!BufferRegistry.has(Command.Name)
      || !BufferRegistry[Command.Name].contains(ArgNo))
    return llvm::createStringError(llvm::inconvertibleErrorCode(),
                                   "No buffer information");

  return extractBuffer(Command.Arguments[ArgNo]);
}
} // namespace revng::tracing
