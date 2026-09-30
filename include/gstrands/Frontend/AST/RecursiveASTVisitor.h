//===-- RecursiveASTVisitor.h - Generate GameplayStrands AST node tables------===//
//
// Derived from clang/AST/RecursiveASTVisitor.h,
// originally part of the LLVM Project.
//
// The upstream code is licensed under Apache License v2.0 with LLVM
// Exceptions. See LICENSES/LLVM.txt and https://llvm.org/LICENSE.txt.
//
// GameplayStrands modifications are licensed under Apache License v2.0.
// See LICENSE.
//
//===----------------------------------------------------------------------===//

#pragma once

#include "gstrands/Frontend/AST/ASTContext.h"
#include "gstrands/Frontend/AST/Decl.h"
#include "gstrands/Frontend/AST/DeclBase.h"

#include "llvm/Support/Casting.h"

// A helper macro to implement short-circuiting when recursing.  It
// invokes CALL_EXPR, which must be a method call, on the derived
// object (s.t. a user of RecursiveASTVisitor can override the method
// in CALL_EXPR).
#define TRY_TO(CALL_EXPR)                                                                                              \
  do {                                                                                                                 \
    if (!getDerived().CALL_EXPR)                                                                                       \
      return false;                                                                                                    \
  } while (false)


namespace gstrands {
class ASTContext;

template <typename Derived> class RecursiveASTVisitor {
public:
  Derived &getDerived() { return *static_cast<Derived *>(this); }

  bool traverseAST(const ASTContext &AST);

  /// Return whether this visitor should traverse post-order.
  bool shouldTraversePostOrder() const { return false; }

  bool traverseDecl(const Decl *D);
  bool traverseDeclContext(const DeclContext *DC);

  // Define WalkUpFrom*() and empty Visit*() for all Decl classes.
  bool walkUpFromDecl(const Decl *D) { return getDerived().visitDecl(D); }
  bool visitDecl(const Decl *D) { return true; }
#define DECL(CLASS, BASE)                                                                                              \
  bool walkUpFrom##CLASS##Decl(const CLASS##Decl *D) {                                                                 \
    TRY_TO(walkUpFrom##BASE(D));                                                                                       \
    TRY_TO(visit##CLASS##Decl(D));                                                                                     \
    return true;                                                                                                       \
  }                                                                                                                    \
  bool visit##CLASS##Decl(const CLASS##Decl *D) { return true; }
#include "gstrands/Frontend/AST/DeclNodes.inc"

#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE) bool traverse##CLASS##Decl(const CLASS##Decl *D);
#include "gstrands/Frontend/AST/DeclNodes.inc"
};

template <typename Derived> bool RecursiveASTVisitor<Derived>::traverseAST(const ASTContext &AST) {
  return getDerived().traverseDecl(AST.getRootDecl());
}

template <typename Derived> bool RecursiveASTVisitor<Derived>::traverseDecl(const Decl *D) {
  switch (D->getKind()) {
#define ABSTRACT_DECL(DECL)
#define DECL(CLASS, BASE)                                                                                              \
  case Decl::CLASS:                                                                                                    \
    if (!getDerived().traverse##CLASS##Decl(static_cast<const CLASS##Decl *>(D)))                                      \
      return false;                                                                                                    \
    break;
#include "gstrands/Frontend/AST/DeclNodes.inc"

  default:
    return true;
  }
  return true;
}


template <typename Derived> bool RecursiveASTVisitor<Derived>::traverseDeclContext(const DeclContext *DC) {
  if (!DC)
    return true;

  for (auto *Child : DC->decls()) {
    TRY_TO(traverseDecl(Child));
  }

  return true;
}

// This macro makes available a variable D, the passed-in decl.
#define DEF_TRAVERSE_DECL(DECL, CODE)                                                                                  \
  template <typename Derived> bool RecursiveASTVisitor<Derived>::traverse##DECL(const DECL *D) {                       \
    bool ShouldVisitChildren = true;                                                                                   \
    bool ReturnValue = true;                                                                                           \
    if (!getDerived().shouldTraversePostOrder())                                                                       \
      TRY_TO(walkUpFrom##DECL(D));                                                                                     \
    {                                                                                                                  \
      CODE;                                                                                                            \
    }                                                                                                                  \
    if (ReturnValue && ShouldVisitChildren)                                                                            \
      TRY_TO(traverseDeclContext(llvm::dyn_cast<const DeclContext>(D)));                                               \
    if (ReturnValue && getDerived().shouldTraversePostOrder())                                                         \
      TRY_TO(walkUpFrom##DECL(D));                                                                                     \
    return ReturnValue;                                                                                                \
  }

DEF_TRAVERSE_DECL(RootDecl, {});
DEF_TRAVERSE_DECL(NamespaceDecl, {});
DEF_TRAVERSE_DECL(ChannelDecl, {});
DEF_TRAVERSE_DECL(TraitDecl, {});
DEF_TRAVERSE_DECL(ComponentDecl, {});
DEF_TRAVERSE_DECL(ImplDecl, {});
DEF_TRAVERSE_DECL(FieldDecl, {});
DEF_TRAVERSE_DECL(VarDecl, {});
DEF_TRAVERSE_DECL(ParmVarDecl, {});

} // namespace gstrands
