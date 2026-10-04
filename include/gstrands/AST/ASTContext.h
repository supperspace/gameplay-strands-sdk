#pragma once

#include "gstrands/Basic/SourceLocation.h"
#include "gstrands/AST/Decl.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Allocator.h"

namespace gstrands {

class ASTContext {
public:
  explicit ASTContext(const SourceLocation RootL);

  template <typename T, typename... Args> T &emplace(Args &&...As) {
    static_assert(std::is_trivially_destructible_v<T>, "AST nodes must not require destruction");
    T *Addr = Alloc.Allocate<T>(1);
    new (Addr) T(std::forward<Args>(As)...);
    return *Addr;
  }

  template <typename T>
  llvm::ArrayRef<T> copyArray(llvm::ArrayRef<T> In) {
    static_assert(std::is_trivially_destructible_v<T>, "Array elements must not require destruction");
    return In.copy(Alloc);
  }

  RootDecl* getRootDecl() {
    return Root;
  }

  const RootDecl* getRootDecl() const {
    return Root;
  }

private:
  llvm::BumpPtrAllocator Alloc;
  RootDecl *Root = nullptr;
};

} // namespace gstrands