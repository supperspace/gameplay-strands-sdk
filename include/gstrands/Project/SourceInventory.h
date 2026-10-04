#pragma once
#include "llvm/Support/VirtualFileSystem.h"

namespace gstrands {

struct SourceInventoryItem {
  llvm::vfs::Status Status;
  std::string NormPath;
  std::array<uint8_t, 32> Blake3Digest;
};

/// Describes a full set of source files that a project includes, at a given time.
class SourceInventory {
public:

  explicit SourceInventory(std::vector<SourceInventoryItem> Items)
    : SourceItems(std::move(Items)) {}

private:
  std::vector<SourceInventoryItem> SourceItems;
};

} // namespace gstrands