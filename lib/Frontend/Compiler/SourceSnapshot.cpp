#include "gstrands/Frontend/Compiler/SourceSnapshot.h"
#include "gstrands/Frontend/Basic/SourceManager.h"

#include "llvm/Support/VirtualFileSystem.h"

gstrands::SourceSnapshot::SourceSnapshot(SourceSnapshot &&) noexcept = default;
gstrands::SourceSnapshot &gstrands::SourceSnapshot::operator=(SourceSnapshot &&) noexcept = default;

gstrands::SourceSnapshot::SourceSnapshot(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS)
  : Filesystem(FS) {}

gstrands::SourceSnapshot::~SourceSnapshot() = default;