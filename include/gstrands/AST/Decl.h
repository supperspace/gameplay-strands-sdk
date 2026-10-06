#pragma once

#include "gstrands/Basic/IdentifierTable.h"
#include "gstrands/AST/DeclBase.h"

namespace gstrands {

// NOLINTNEXTLINE(misc-multiple-inheritance)
class RootDecl : public Decl, public DeclContext {
public:
  static bool classof(const Decl *D) { return D->getKind() == Root; }

  explicit RootDecl(const SourceLocation L) : Decl(Root, L) {}
};

class NamedDecl : public Decl {
public:
  static bool classof(const Decl *D) { return D->getKind() >= firstNamed && D->getKind() <= lastNamed; }

  llvm::StringRef getNameSpelling() const { return Ident.getSpelling(); }

  Identifier getIdentifier() const { return Ident; }

protected:
  NamedDecl(const Kind K, const SourceLocation L, const Identifier I) : Decl(K, L), Ident(I) {}
  Identifier Ident;
};

// NOLINTNEXTLINE(misc-multiple-inheritance)
class NamespaceDecl : public NamedDecl, public DeclContext {
public:
  static bool classof(const Decl *D) { return D->getKind() == Namespace; }

  NamespaceDecl(const SourceLocation L, const llvm::ArrayRef<Identifier> QualName)
      : NamedDecl(Namespace, L, QualName.back()) {}

protected:
  llvm::ArrayRef<Identifier> QualifiedName;
};

class ChannelDecl : public NamedDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Channel; }

  ChannelDecl(const SourceLocation L, const Identifier I) : NamedDecl(Channel, L, I) {}
};

// NOLINTNEXTLINE(misc-multiple-inheritance)
class RecordDecl : public NamedDecl, public DeclContext {
public:
  static bool classof(const Decl *D) { return D->getKind() >= firstRecord && D->getKind() <= lastRecord; }

protected:
  using NamedDecl::NamedDecl;
};

class ComponentDecl : public RecordDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Component; }

  ComponentDecl(const SourceLocation L, const Identifier I) : RecordDecl(Component, L, I) {}
};

class TraitDecl : public RecordDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Trait; }

  TraitDecl(const SourceLocation L, const Identifier I) : RecordDecl(Trait, L, I) {}
};

class ImplDecl : public Decl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Impl; }

  explicit ImplDecl(const SourceLocation L, const llvm::ArrayRef<Identifier> QualTrait,
                    const llvm::ArrayRef<Identifier> QualImplementer)
      : Decl(Impl, L), QualifiedTrait(QualTrait), QualifiedImplementer(QualImplementer) {}

  llvm::StringRef getTraitNameSpelling() const { return QualifiedTrait.back().getSpelling(); }
  llvm::StringRef getImplementerNameSpelling() const { return QualifiedImplementer.back().getSpelling(); }

private:
  llvm::ArrayRef<Identifier> QualifiedTrait;
  llvm::ArrayRef<Identifier> QualifiedImplementer;
};

class ValueDecl : public NamedDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() >= firstValue && D->getKind() <= lastValue; }

protected:
  using NamedDecl::NamedDecl;
};

class PropertyDecl : public ValueDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Property; }

  explicit PropertyDecl(const SourceLocation L, const Identifier I) : ValueDecl(Property, L, I) {}
};

class VarDecl : public ValueDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Var; }

  explicit VarDecl(const SourceLocation L, const Identifier I) : ValueDecl(Var, L, I) {}
};

class ParmVarDecl : public ValueDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == ParmVar; }

  explicit ParmVarDecl(const SourceLocation L, const Identifier I) : ValueDecl(ParmVar, L, I) {}
};

} // namespace gstrands