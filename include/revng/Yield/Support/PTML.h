#pragma once

//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

#include <algorithm>
#include <climits>

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"

#include "revng/ADT/Concepts.h"
#include "revng/Support/Debug.h"

namespace yield::ptml {

namespace tags {

static constexpr auto Div = "div";
static constexpr auto Span = "span";

} // namespace tags

namespace attributes {

static constexpr auto scope = "data-scope";
static constexpr auto token = "data-token";
static constexpr auto locationDefinition = "data-location-definition";
static constexpr auto locationReferences = "data-location-references";
static constexpr auto modelEditPath = "data-model-edit-path";

} // namespace attributes

namespace scopes {

static constexpr auto Indentation = "indentation";

}

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

// Inspired by mlir::indented_raw_ostream
class PTMLIndentedOstream : public llvm::raw_ostream {
private:
  int IndentSize;
  int IndentDepth;
  bool TrailingNewline;
  raw_ostream &OS;

public:
  explicit PTMLIndentedOstream(llvm::raw_ostream &OS, int IndentSize = 2) :
    IndentSize(IndentSize), IndentDepth(0), TrailingNewline(false), OS(OS) {
    SetUnbuffered();
  }

  struct Scope {
  private:
    PTMLIndentedOstream &OS;

  public:
    explicit Scope(PTMLIndentedOstream &OS) : OS(OS) { OS.indent(); }
    ~Scope() { OS.unindent(); }
  };

  Scope scope() { return Scope(*this); }
  void indent() { IndentDepth = std::min(INT_MAX, IndentDepth + 1); }
  void unindent() { IndentDepth = std::max(0, IndentDepth - 1); }

private:
  void write_impl(const char *Ptr, size_t Size) override;
  uint64_t current_pos() const override;
  void writeIndent();
};

} // namespace yield::ptml
