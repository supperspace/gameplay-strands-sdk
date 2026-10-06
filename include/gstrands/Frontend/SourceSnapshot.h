#pragma once
#include "gstrands/Project/Project.h"

#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace llvm::vfs {
class FileSystem;
} // namespace llvm::vfs

namespace gstrands {

class SourceManager;

struct SourceDraftMetadata {
  int64_t Version;
  /// true if this draft belongs to a project. That a requirement for a valid source file.
  bool ProjectScoped;
};

class SourceSnapshot {
public:
  SourceSnapshot() = delete;

  SourceSnapshot(const SourceSnapshot&) = delete;
  SourceSnapshot(SourceSnapshot&&) noexcept;

  SourceSnapshot& operator=(const SourceSnapshot&) = delete;
  SourceSnapshot& operator=(SourceSnapshot&&) noexcept;

  SourceSnapshot(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS, std::vector<Project> Ps,
                 llvm::StringMap<SourceDraftMetadata> Sources);

  ~SourceSnapshot();

  llvm::ArrayRef<Project> getProjects() const {
    return Projects;
  }

  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> getFilesystem() const;
  SourceManager& getSourceManager() {
    return *SM;
  }

  const auto& getSourceDrafts() const {
    return SourceDrafts;
  }
private:
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> Filesystem;
  std::unique_ptr<SourceManager> SM;
  std::vector<Project> Projects;
  llvm::StringMap<SourceDraftMetadata> SourceDrafts;
};
} // namespace gstrands