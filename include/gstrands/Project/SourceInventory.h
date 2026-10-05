#pragma once
#include "gstrands/Core/SourceFile.h"

#include "llvm/ADT/StringMap.h"

#include <ranges>

namespace gstrands {

/// Describes a full set of source files that a project includes, at a given time.
class SourceInventory {
public:

  const auto& getSources() const {
    return Sources;
  }

  void addSource(llvm::StringRef CanonicalPath, SourceFileSignature Signature);

private:
  llvm::StringMap<SourceFileSignature> Sources;
};

} // namespace gstrands