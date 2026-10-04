#pragma once

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"

#include "gstrands/Vocab/TokenDefs.h"

namespace gstrands {

class IdentifierInfo {
  friend class IdentifierTable;
public:
  tok::TokenKind K = tok::Identifier;

  llvm::StringMapEntry<IdentifierInfo *> *Entry = nullptr;
};

class Identifier {
public:
  Identifier() = default;

  llvm::StringRef getSpelling() const {
    assert(Info != nullptr);
    return Info->Entry->first();
  }

  tok::TokenKind getTokenKind() const {
    return Info ? Info->K : tok::Invalid;
  }

  friend bool operator==(Identifier Lhs, Identifier Rhs) {
    return Lhs.Info == Rhs.Info;
  }

  explicit operator bool() const { return Info != nullptr; }

private:
  friend class IdentifierTable;
  explicit Identifier(IdentifierInfo &Info) : Info(&Info) {}
  IdentifierInfo *Info = nullptr;
};

class IdentifierTable {
  using HashTableTy =
      llvm::StringMap<IdentifierInfo *, llvm::BumpPtrAllocator &>;
  HashTableTy HashTable;

public:
  explicit IdentifierTable(llvm::BumpPtrAllocator &Alloc) : HashTable(Alloc) {
    addKeywords();
  }

  IdentifierTable(const IdentifierTable &Table) = delete;
  IdentifierTable &operator=(const IdentifierTable &Table) = delete;

  IdentifierTable(IdentifierTable &&Table) = delete;
  IdentifierTable &operator=(IdentifierTable &&Table) = delete;

  Identifier get(const llvm::StringRef Name) {
    auto &Entry = *HashTable.try_emplace(Name, nullptr).first;

    IdentifierInfo *&II = Entry.second;
    if (II)
      return Identifier(*II);

    // Lookups failed, make a new IdentifierInfo.
    void *Mem = getAllocator().Allocate<IdentifierInfo>();
    II = new (Mem) IdentifierInfo();

    // Make sure getName() knows how to find the IdentifierInfo
    // contents.
    II->Entry = &Entry;

    return Identifier(*II);
  }

  llvm::BumpPtrAllocator &getAllocator() { return HashTable.getAllocator(); }

private:
  void addKeywords();
};

} // namespace gstrands