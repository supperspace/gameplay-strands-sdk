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
    explicit Project(llvm::StringRef BaseDir);

    llvm::StringRef getBaseDirectory() const {
      return BaseDirectory;
    }

    /// Creates a source inventory in a given filesystem context. The source inventory is only valid at the time
    /// of its creation, and there is no reliable way of checking if the filesystem state has changed, other than
    /// creating another inventory and comparing them
    llvm::Expected<SourceInventory> createSourceInventory(llvm::vfs::FileSystem& FS) const;

    std::strong_ordering operator<=>(const Project &) const = default;
  private:
    llvm::SmallString<512> BaseDirectory;
  };

} // namespace gstrands