#pragma once

#include "gstrands/Project/SourceInventory.h"

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Error.h"

namespace llvm::vfs {
class FileSystem;
} // namespace llvm::vfs

namespace gstrands {

  class Project {
  public:
    Project(llvm::StringRef BaseDir, llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> F);

    llvm::StringRef getBaseDirectory() const {
      return BaseDirectory;
    }

    /// Creates a source inventory in a given filesystem context. The source inventory is only valid at the time
    /// of its creation, and there is no reliable way of checking if the filesystem state has changed, other than
    /// creating another inventory and comparing them
    llvm::Expected<SourceInventory> createSourceInventory() const;

  private:
    llvm::SmallString<512> BaseDirectory;
    std::error_code BaseDirNormalizationEC;
    llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS;
  };

} // namespace gstrands