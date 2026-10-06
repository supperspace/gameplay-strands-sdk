#include "gstrands/Frontend/Workspace.h"

#include "gstrands/Basic/SourceManager.h"

#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/VirtualFileSystem.h"

#include <set>

using namespace llvm;
namespace gstrands {

Workspace::Workspace() = default;
Workspace::Workspace(Workspace &&) noexcept = default;
Workspace &Workspace::operator=(Workspace &&) noexcept = default;

Workspace::~Workspace() = default;

SourceSnapshot Workspace::fileOpened(const StringRef Path, const StringRef Contents, const int64_t Ver) {
  // The file should not exist in our file map yet.
  SmallString<256> Normalized;
  sys::fs::real_path(Path, Normalized);

  assert(!OpenFileMap.contains(Normalized));

  updateFile(Normalized, Contents, Ver).UriPath = Path;
  return assembleSourceSnapshot();
}

SourceSnapshot Workspace::fileChanged(const StringRef Path, const StringRef Contents, const int64_t Ver) {
  SmallString<256> Normalized;
  sys::fs::real_path(Path, Normalized);

  assert(OpenFileMap.contains(Normalized));

  updateFile(Normalized, Contents, Ver).UriPath = Path;
  return assembleSourceSnapshot();
}

SourceSnapshot Workspace::fileSaved(const StringRef Path) {
  SmallString<256> Normalized;
  sys::fs::real_path(Path, Normalized);

  assert(OpenFileMap.contains(Normalized));
  return assembleSourceSnapshot();
}

SourceSnapshot Workspace::fileClosed(const StringRef Path) {
  SmallString<256> Normalized;
  sys::fs::real_path(Path, Normalized);


  assert(OpenFileMap.contains(Normalized));
  OpenFileMap.erase(Normalized);
  return assembleSourceSnapshot();
}

std::string Workspace::getDraftCopy(const StringRef Path) const {
  SmallString<256> Normalized;
  sys::fs::real_path(Path, Normalized);

  if (auto I = OpenFileMap.find(Path); I != OpenFileMap.end()) {
    return I->getValue().Content;
  }
  return {};
}

SourceFileDraft & Workspace::updateFile(const StringRef Path, const StringRef Contents, const int64_t Ver) {
  auto &Draft = OpenFileMap[Path];
  Draft.Content = Contents;
  Draft.Timestamp = std::chrono::system_clock::now();
  Draft.Version = Ver;
  return Draft;
}

SourceSnapshot Workspace::assembleSourceSnapshot() {
  const IntrusiveRefCntPtr OverlayFS(new vfs::OverlayFileSystem(vfs::getRealFileSystem()));
  const IntrusiveRefCntPtr MemoryFS(new vfs::InMemoryFileSystem());

  OverlayFS->pushOverlay(MemoryFS);

  std::set<Project> Projects;
  StringMap<SourceDraftMetadata> DraftMetadataMap;

  for (const auto &[P, FS] : OpenFileMap) {
    std::string BufferName = formatv("{0}::{1}", FS.Version, P);
    auto Buff = MemoryBuffer::getMemBufferCopy(FS.Content, BufferName);
    MemoryFS->addFile(P, sys::toTimeT(FS.Timestamp), std::move(Buff));
  }

  for (const auto &[Path, FS] : OpenFileMap) {
    auto &DraftMetadata = DraftMetadataMap[Path];
    DraftMetadata.Version = FS.Version;

    if (auto P = discoverSuitableProject(Path, *OverlayFS); !P.getError()) {
      Projects.insert(P.get());
      DraftMetadata.ProjectScoped = true;
    } else {
      DraftMetadata.ProjectScoped = false;
    }
  }

  return SourceSnapshot(OverlayFS, {Projects.begin(), Projects.end()}, std::move(DraftMetadataMap));
}

ErrorOr<Project> Workspace::discoverSuitableProject(const StringRef SrcPath, vfs::FileSystem &FS) {
  auto Directory = sys::path::parent_path(SrcPath);

  std::error_code EC;
  while (!Directory.empty()) {
    auto It = FS.dir_begin(Directory, EC);
    const vfs::directory_iterator End;

    while (!EC && It != End) {
      if (sys::path::extension(It->path()) == ".gsp")
        return Project(sys::path::parent_path(It->path()));
      It.increment(EC);
    }

    if (EC)
      return EC;

    Directory = sys::path::parent_path(Directory);
  }

  return errc::no_such_file_or_directory;
}

} // namespace gstrands