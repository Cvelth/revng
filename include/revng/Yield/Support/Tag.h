#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"

#include "revng/ADT/Concepts.h"
#include "revng/Support/Debug.h"

namespace yield::htmlTag {

class Tag {
private:
  std::string TheTag;
  std::string Content;
  llvm::StringMap<std::string> Attributes;

public:
  explicit Tag(llvm::StringRef Tag, llvm::StringRef Content = "") :
    TheTag(Tag.str()), Content(Content.str()) {}

  Tag &add(llvm::StringRef Name, llvm::StringRef Value) {
    Attributes[Name] = Value.str();
    return *this;
  }

  template<::ranges::typed<llvm::StringRef> T>
  Tag &add(llvm::StringRef Name, const T &Values) {
    Attributes[Name] = llvm::join(Values, ",");
    return *this;
  }

  template<typename... T>
  requires(std::is_convertible_v<llvm::StringRef, T> and...)
    Tag &add(llvm::StringRef Name, T &&...Values) {
    Attributes[Name] = llvm::join(",", std::forward<T>(Values)...);
    return *this;
  }

  std::string serialize() const {
    llvm::SmallString<128> Out;
    Out.append({ "<", TheTag, " " });
    for (auto &Pair : Attributes)
      Out.append({ Pair.first(), "=\"", Pair.second, "\" " });
    Out.append({ ">", Content, "</", TheTag, ">" });
    return Out.str().str();
  }

  void dump() const debug_function { dump(dbg); }

  template<typename T>
  void dump(T &Output) const {
    Output << serialize();
  }
};

} // namespace yield::htmlTag
