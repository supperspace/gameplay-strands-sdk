#include "gstrands/Basic/SourceManager.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

namespace gstrands {

SourceManager::SourceManager(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS)
  : Filesystem(FS) {}

SourceManager::~SourceManager() = default;

llvm::Expected<SourceBufferView>
SourceManager::getOrLoadBuffer(const llvm::StringRef Path) {
  if (const auto ExistingIter = FilenameLookup.find(Path);
      ExistingIter == FilenameLookup.end()) {

    // Buffer not found. Open the file anew.
    auto MemoryBuffer = Filesystem->getBufferForFile(Path);
    if (const std::error_code EC = MemoryBuffer.getError(); EC) {
      return llvm::errorCodeToError(EC);
    }

    const size_t FileSize = MemoryBuffer.get()->getBufferSize();

    const uint32_t BaseOffset = SourceLocBaseOffset;
    const uint64_t Remaining = uint64_t{UINT32_MAX} - BaseOffset;

    // Leave room for the next base offset.
    if (FileSize >= Remaining)
      return llvm::createStringError(
          llvm::errc::file_too_large,
          "Source location address space exhausted");

    const uint32_t EndOffset =
        BaseOffset + static_cast<uint32_t>(FileSize);

    SourceLocBaseOffset = EndOffset + 1;

    FileID NewSourceId = Sources.size();
    Sources.emplace_back(BaseOffset, EndOffset, std::move(MemoryBuffer.get()));

    FilenameLookup[Path] = NewSourceId;
    SourceMap.insert(BaseOffset, EndOffset, NewSourceId);

    const auto &NewSourceInfo = Sources[NewSourceId];

    return SourceBufferView{
        .BufferRef = NewSourceInfo.Buffer->getMemBufferRef(),
        .BaseLocation = SourceLocation{NewSourceInfo.StartOffset}};
  } else {
    const auto &SourceInfo = Sources[ExistingIter->second];
    return SourceBufferView{.BufferRef = SourceInfo.Buffer->getMemBufferRef(),
                            .BaseLocation =
                                SourceLocation{SourceInfo.StartOffset}};
  }
}
} // namespace gstrands