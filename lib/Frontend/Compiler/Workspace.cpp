#include "gstrands/Frontend/Compiler/Workspace.h"

#include "llvm/Support/VirtualFileSystem.h"

using namespace llvm;
namespace gstrands {

SourceSnapshot Workspace::fileOpened(const StringRef Path, const StringRef Contents) {
  // The file should not exist in our file map yet.
  assert(!OpenFileMap.contains(Path));

  const IntrusiveRefCntPtr NewFS(new vfs::InMemoryFileSystem());

  NewFS->addFile(Path, 0, MemoryBuffer::getMemBufferCopy(Contents));
  OpenFileMap[Path] = NewFS;

  return assembleSourceSnapshot();
}

SourceSnapshot Workspace::assembleSourceSnapshot() {
  const IntrusiveRefCntPtr OverlayFS(new vfs::OverlayFileSystem(vfs::getRealFileSystem()));

  for (const auto&[P, FS]: OpenFileMap) {
    OverlayFS->pushOverlay(FS);
  }

  SourceSnapshot R(OverlayFS);
  return R;
}

} // namespace gstrands