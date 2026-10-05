#include "gstrands/Project/Project.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/BLAKE3.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/Support/raw_ostream.h"

#include <set>
#include <system_error>

namespace gstrands {

Project::Project(const llvm::StringRef BaseDir)
  : BaseDirectory(BaseDir)
{}

llvm::Expected<SourceInventory> Project::createSourceInventory(llvm::vfs::FileSystem& FS) const {
  llvm::SmallString<512> RealBaseDir;
  std::error_code EC = FS.getRealPath(BaseDirectory, RealBaseDir);

  if (EC)
    return llvm::errorCodeToError(EC);

  llvm::vfs::recursive_directory_iterator End;
  llvm::vfs::recursive_directory_iterator Iterator(FS, getBaseDirectory(), EC);
  if (EC) {
    return llvm::errorCodeToError(EC);
  }

  SourceInventory Inventory;
  while (Iterator != End) {
    if (Iterator->type() == llvm::sys::fs::file_type::regular_file) {
      if (llvm::sys::path::extension(Iterator->path()) == ".gss") {

        // Start by normalizing the path
        llvm::SmallString<256> Resolved;

        if (EC = FS.getRealPath(Iterator->path(), Resolved); EC)
          return llvm::errorCodeToError(EC);

        auto R = FS.status(Iterator->path());
        if (EC = R.getError(); EC)
          return llvm::errorCodeToError(EC);

        SourceFileSignature Signature;
        Signature.FileSize = R.get().getSize();
        Signature.Timestamp = R.get().getLastModificationTime();

        // Add it to the input set
        {
          auto ErrorOrBuff =
              FS.getBufferForFile(Iterator->path(), -1, false, false, false);

          if (EC = ErrorOrBuff.getError(); EC)
            return llvm::errorCodeToError(EC);

          const auto Buff = std::move(ErrorOrBuff.get());
          llvm::BLAKE3 Blake3;
          Blake3.update(Buff->getBuffer());

          Blake3.final(Signature.Blake3Digest);
        }

        Inventory.addSource(Resolved, Signature);
      }
    }


    Iterator = Iterator.increment(EC);
    if (EC)
      return llvm::errorCodeToError(EC);
  }

  return Inventory;
}

} // namespace gstrands