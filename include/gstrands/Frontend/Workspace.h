#pragma once
#include "gstrands/Frontend/SourceSnapshot.h"
#include "gstrands/Project/Project.h"

#include "llvm/ADT/StringMap.h"

#include <chrono>

namespace gstrands {
class ASTContext;

struct SourceFileDraft {
  std::string Content;
  std::chrono::sys_time<std::chrono::nanoseconds> Timestamp;
  int64_t Version = 0;
};

class Workspace {
public:
  Workspace();
  Workspace(const Workspace&) = delete;
  Workspace(Workspace&&) noexcept;

  Workspace& operator=(const Workspace&) = delete;
  Workspace& operator=(Workspace&&) noexcept;

  ~Workspace();

  SourceSnapshot fileOpened(llvm::StringRef Path, llvm::StringRef Contents, int64_t Ver);
  SourceSnapshot fileChanged(llvm::StringRef Path, llvm::StringRef Contents, int64_t Ver);
  SourceSnapshot fileSaved(llvm::StringRef Path);
  SourceSnapshot fileClosed(llvm::StringRef Path);

  std::string getDraftCopy(llvm::StringRef Path) const;

private:
  void updateFile(llvm::StringRef Path, llvm::StringRef Contents, int64_t Ver);
  SourceSnapshot assembleSourceSnapshot();
  static llvm::ErrorOr<Project> discoverSuitableProject(llvm::StringRef SrcPath, llvm::vfs::FileSystem& FS);

  // Per open file we maintain a table that holds an in memory filesystem with that files last known source content
  llvm::StringMap<SourceFileDraft> OpenFileMap;

  std::string TopMostParentPath;
};

} // namespace gstrands