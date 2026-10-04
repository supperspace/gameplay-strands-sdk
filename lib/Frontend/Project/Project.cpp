#include "gstrands/Frontend/Project/Project.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/DebugInfo/LogicalView/Core/LVSupport.h"
#include "llvm/Support/BLAKE3.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

#include <set>
#include <system_error>

namespace gstrands {

Project::Project(const llvm::StringRef BaseDir, llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> F)
  : FS(F) {
  BaseDirNormalizationEC = FS->getRealPath(BaseDir, BaseDirectory);
}

llvm::Expected<SourceInventory> Project::createSourceInventory() const {
  if (BaseDirNormalizationEC)
    return llvm::errorCodeToError(BaseDirNormalizationEC);

  std::error_code EC;

  llvm::vfs::recursive_directory_iterator End;
  llvm::vfs::recursive_directory_iterator Iterator(*FS, getBaseDirectory(), EC);
  if (EC) {
    return llvm::errorCodeToError(EC);
  }


  std::vector<SourceInventoryItem> SourceInvItems;

  while (Iterator != End) {
    if (Iterator->type() == llvm::sys::fs::file_type::regular_file) {
      if (llvm::sys::path::extension(Iterator->path()) == ".gss") {

        // Start by normalizing the path
        llvm::SmallString<256> Resolved;

        if (EC = FS->getRealPath(Iterator->path(), Resolved); EC)
          return llvm::errorCodeToError(EC);

        auto R = FS->status(Iterator->path());
        if (EC = R.getError(); EC)
          return llvm::errorCodeToError(EC);

        // Add it to the input set
        SourceInvItems.emplace_back(R.get(), Resolved.c_str());
        {
          auto ErrorOrBuff =
              FS->getBufferForFile(Iterator->path(), -1, false, false, false);

          if (EC = ErrorOrBuff.getError(); EC)
            return llvm::errorCodeToError(EC);

          const auto Buff = std::move(ErrorOrBuff.get());
          llvm::BLAKE3 Blake3;
          Blake3.update(Buff->getBuffer());

          Blake3.final(SourceInvItems.back().Blake3Digest);
        }

        llvm::errs() << "Source: '" << SourceInvItems.back().NormPath << "'\nDigest: '"
                     << llvm::toHex(SourceInvItems.back().Blake3Digest, true) << "'\n\n";
      }
    }


    Iterator = Iterator.increment(EC);
    if (EC)
      return llvm::errorCodeToError(EC);
  }

  return SourceInventory{std::move(SourceInvItems)};
}

} // namespace gstrands