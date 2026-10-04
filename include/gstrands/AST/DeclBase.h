#pragma once

#include "gstrands/Basic/SourceLocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/Casting.h"

namespace gstrands {

class DeclContext;
class Decl {
public:
  enum Kind : uint8_t {
    Unknown,
#define ABSTRACT_DECL(...)
#define DECL(Name, ...) Name,
#include "gstrands/AST/DeclNodes.inc"

#define ABSTRACT_DECL(...)
#define DECL(...)
#define DECL_RANGE(Base, First, Last) first##Base = First, last##Base = Last,
#include "gstrands/AST/DeclNodes.inc"

  };

  DeclContext *asDeclContext();
  const DeclContext *asDeclContext() const { return const_cast<Decl*>(this)->asDeclContext(); }

  bool isDeclContext() const {
    switch (getKind()) {
#define ABSTRACT_DECL(...)
#define DECL(...)
#define DECL_CONTEXT(Name) case Name: return true;
#include "gstrands/AST/DeclNodes.inc"
    default: return false;
    }
  }

  Kind getKind() const { return DeclKind; }

protected:
  Decl(const Kind K, const SourceLocation L) : DeclKind(K), Loc(L) {}

private:
  Kind DeclKind = Unknown;
  SourceLocation Loc;
};


class DeclContext {
public:
  static bool classof(const Decl *D) { return D->isDeclContext(); }

  void setChildren(const llvm::ArrayRef<Decl*> C) {
    Children = C;
  }

  llvm::ArrayRef<Decl*> decls() { return Children; }
  llvm::ArrayRef<const Decl*> decls() const { return Children; }

private:
  llvm::ArrayRef<Decl *> Children;
};

} // namespace gstrands

template <typename To, typename From>
struct llvm::CastInfo<To, From *,
                      std::enable_if_t<std::is_same_v<std::remove_const_t<To>, gstrands::DeclContext> &&
                                       std::is_base_of_v<gstrands::Decl, From>>> {

  using Result = std::conditional_t<std::is_const_v<To> || std::is_const_v<From>, const gstrands::DeclContext *,
                                    gstrands::DeclContext *>;

  static bool isPossible(From *D) {
    assert(D && "isa on a null decl");
    return gstrands::DeclContext::classof(D);
  }

  static Result doCast(From *D) { return D->asDeclContext(); }
  static Result castFailed() { return nullptr; }
  static Result doCastIfPossible(From *D) { return D->asDeclContext(); }
};

template <typename To, typename From>
struct llvm::CastInfo<To, From,
                      std::enable_if_t<std::is_same_v<std::remove_const_t<To>, gstrands::DeclContext> &&
                                       std::is_base_of_v<gstrands::Decl, From>>> {
  using PointerCast = CastInfo<To, From *>;

  static bool isPossible(const From &D) { return gstrands::DeclContext::classof(&D); }
  static decltype(auto) doCast(From &D) { return *PointerCast::doCast(&D); }
  static auto castFailed() { return PointerCast::castFailed(); }
  static auto doCastIfPossible(From &D) { return PointerCast::doCastIfPossible(&D); }
};
