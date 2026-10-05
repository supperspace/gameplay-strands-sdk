#include "gstrands/Project/SourceInventory.h"

namespace gstrands {

void SourceInventory::addSource(llvm::StringRef CanonicalPath, SourceFileSignature Signature) {
  Sources[CanonicalPath] = Signature;
}

} // namespace gstrands