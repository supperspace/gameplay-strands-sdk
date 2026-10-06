#pragma once
#include "gstrands/Basic/SourceLocation.h"

#include "llvm/ADT/IntervalMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/MemoryBuffer.h"

namespace llvm::vfs {
class FileSystem;
}
namespace gstrands {


struct SourceBufferView {
  llvm::MemoryBufferRef BufferRef;
  SourceLocation BaseLocation;
};

class SourceManager {
public:
  explicit SourceManager(llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS);
  ~SourceManager();

  SourceManager(const SourceManager &) = delete;
  SourceManager &operator=(const SourceManager &) = delete;
  SourceManager(SourceManager &&) = delete;
  SourceManager &operator=(SourceManager &&) = delete;

  using FileID = uint32_t;

  llvm::Expected<SourceBufferView> getOrLoadBuffer(llvm::StringRef SourceFile);

  FileID getFileForLocation(SourceLocation L) const;
  FileID getFileId(llvm::StringRef Path) const;

  ExpandedSourceLocation expandSourceLocation(SourceLocation L) const;

private:
  struct SourceInfo {
    SourceInfo(const SourceInfo&) = delete;
    SourceInfo& operator=(const SourceInfo&) = delete;

    SourceInfo(SourceInfo&&) = default;
    SourceInfo& operator=(SourceInfo&&) = default;

    SourceInfo(const uint32_t InBaseOffset, const uint32_t InEndOffset,
               std::unique_ptr<llvm::MemoryBuffer> InBuffer)
        : StartOffset(InBaseOffset), EndOffset(InEndOffset),
          Buffer(std::move(InBuffer)) {}

    uint32_t StartOffset = 0;
    uint32_t EndOffset = 0;
    std::unique_ptr<llvm::MemoryBuffer> Buffer;

    mutable std::vector<uint32_t> LazyCachedLineOffsets;
  };

  void populateLineOffsets(FileID File) const;

  using SourceLocationRangeMap = llvm::IntervalMap<uint32_t, FileID>;

  std::vector<SourceInfo> Sources;
  SourceLocationRangeMap::Allocator SourceMapAllocator;
  SourceLocationRangeMap SourceMap{SourceMapAllocator};
  llvm::StringMap<FileID> FilenameLookup;
  uint32_t SourceLocBaseOffset = 1;
  llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> Filesystem;
};

} // namespace gstrands