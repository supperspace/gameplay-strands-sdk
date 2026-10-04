#pragma once
#include "gstrands/Frontend/SourceSnapshot.h"

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringMap.h"

namespace llvm::vfs {
class InMemoryFileSystem;
} // namespace llvm::vfs

namespace gstrands {
class ASTContext;

class SourceVersion {
public:

private:
  size_t Version = 0;
  std::unique_ptr<ASTContext> AST;
  std::string Source;
};

class Workspace {
public:
  Workspace();
  Workspace(const Workspace&) = delete;
  Workspace(Workspace&&) noexcept;

  Workspace& operator=(const Workspace&) = delete;
  Workspace& operator=(Workspace&&) noexcept;

  ~Workspace();

  SourceSnapshot fileOpened(llvm::StringRef Path, llvm::StringRef Contents);
  SourceSnapshot fileChanged(llvm::StringRef Path);
  SourceSnapshot fileSaved(llvm::StringRef Path);
  SourceSnapshot fileClosed(llvm::StringRef Path);

private:
  SourceSnapshot assembleSourceSnapshot();

  // Per open file we maintain a table that holds an in memory filesystem with that files last known source content
  llvm::StringMap<llvm::IntrusiveRefCntPtr<llvm::vfs::InMemoryFileSystem>> OpenFileMap;
};

} // namespace gstrands