#pragma once
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace llvm::vfs {
class FileSystem;
} // namespace llvm::vfs

namespace gstrands {

class SourceManager;

class SourceSnapshot {
public:
  SourceSnapshot() = delete;

  SourceSnapshot(const SourceSnapshot&) = delete;
  SourceSnapshot(SourceSnapshot&&) noexcept;

  SourceSnapshot& operator=(const SourceSnapshot&) = delete;
  SourceSnapshot& operator=(SourceSnapshot&&) noexcept;

  explicit SourceSnapshot(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS);
  ~SourceSnapshot();
private:
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> Filesystem;
  std::unique_ptr<SourceManager> SM;
};
} // namespace gstrands