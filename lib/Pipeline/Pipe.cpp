/// \file Pipe.cpp

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "revng/Pipeline/LLVMContainer.h"
#include "revng/Pipeline/Pipe.h"

using namespace pipeline;

using llvm::StringRef;

void dumpNextContainer(pipeline::ContainerSet const &Containers,
                       llvm::StringRef Name) {
  static constexpr llvm::StringRef GenericPipeName = "generic-llvm-pipe";
  if (Name.starts_with(GenericPipeName))
    return;

  if (Containers.contains("module.bc.zstd")) {
    auto const &C = Containers.get<pipeline::LLVMContainer>("module.bc.zstd");
    dumpNext(C.getModule(), "(pipe) " + Name.str());
  } else {
    dbg << "No `module.bc.zstd` in `" << Name.str() << "`.\n" << std::endl;
  }
}

void InvalidationMetadata::registerTargetsDependingOn(const Context &Context,
                                                      StringRef GlobalName,
                                                      const TupleTreePath &Path,
                                                      ContainerToTargetsMap
                                                        &Out,
                                                      Logger<> &Log) const {
  if (auto Iter = PathCache.find(GlobalName); Iter != PathCache.end()) {

    auto &Bimap = Iter->second;
    auto It = Bimap.find(Path);
    if (It == Bimap.end())
      return;

    if (Log.isEnabled()) {
      Log << "Registering: ";
      for (const auto &Entry : It->second) {
        Log << Entry.getTarget().toString() << " in "
            << Entry.getContainerName() << "\n";
      }
      Log << DoLog;
    }

    for (const auto &Entry : It->second)
      Out.add(Entry.getContainerName(), Entry.getTarget());
  }
}

void InvalidationMetadata::remove(const ContainerToTargetsMap &Map) {
  for (auto &Pair : Map) {
    auto Iter = PathCache.find(Pair.first());
    if (Iter == PathCache.end())
      continue;

    Iter->second.remove(Pair.second, Pair.first());
  }
}

void InvalidationMetadata::dump(const pipeline::Context &Context,
                                unsigned Indentation) const {
  for (const auto &[GlobalName, InvalidationData] : PathCache) {
    indent(dbg, Indentation);
    dbg << "Global " << GlobalName.str() << ":\n";

    for (const auto &[Path, Targets] : PathCache.find(GlobalName)->second) {
      indent(dbg, Indentation + 1);

      dbg << llvm::cantFail(Context.getGlobals().get(GlobalName))
               ->serializePath(Path)
               .value_or("(unavailable)")
          << ":\n";

      for (const TargetInContainer &Target : Targets) {
        Target.dump(dbg, Indentation + 2);
      }
    }
  }
}
