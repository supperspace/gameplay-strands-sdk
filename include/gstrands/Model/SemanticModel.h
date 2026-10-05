#pragma once

#include "gstrands/Core/SourceFile.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gstrands {

/// The compilation result
class SemanticModel {
public:

  std::optional<SourceFileSignature> getSourceSignature(const std::string_view Path) const {
    if (auto Res = SourceFiles.find(Path); Res != SourceFiles.end()) {
      return Res->second;
    }
    return std::nullopt;
  }

private:
  struct StringHash {
    using is_transparent = void;

    std::size_t operator()(std::string_view S) const noexcept {
      return std::hash<std::string_view>{}(S);
    }
  };

  using SourceMap = std::unordered_map<std::string, SourceFileSignature, StringHash, std::equal_to<>>;

  /// The source files that contributed to the creation of this model
  SourceMap SourceFiles;
};

} // namespace gstrands