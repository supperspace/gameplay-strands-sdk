#include "gstrands/Frontend/SourceSnapshot.h"
#include "gstrands/Basic/SourceManager.h"

#include "llvm/Support/VirtualFileSystem.h"

gstrands::SourceSnapshot::SourceSnapshot(SourceSnapshot &&) noexcept = default;
gstrands::SourceSnapshot &gstrands::SourceSnapshot::operator=(SourceSnapshot &&) noexcept = default;

gstrands::SourceSnapshot::SourceSnapshot(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS, std::vector<Project> Ps)
    : Filesystem(FS), SM(std::make_unique<SourceManager>(FS)), Projects(std::move(Ps)) {}

gstrands::SourceSnapshot::~SourceSnapshot() = default;

llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> gstrands::SourceSnapshot::getFilesystem() const {
  return Filesystem;
}