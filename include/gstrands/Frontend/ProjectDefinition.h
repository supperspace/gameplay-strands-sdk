#pragma once

#include "llvm/ADT/StringRef.h"

#include <string>

namespace gstrands {

  class ProjectDefinition {
  public:
    explicit ProjectDefinition(llvm::StringRef BaseDir)
      : BaseDirectory(BaseDir) {}

    llvm::StringRef getBaseDirectory() const {
      return BaseDirectory;
    }

  private:
    std::string BaseDirectory;
  };

} // namespace gstrands