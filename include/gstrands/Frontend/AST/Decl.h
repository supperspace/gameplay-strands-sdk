#pragma once
#include "DeclBase.h"

namespace gstrands {

// NOLINTNEXTLINE(misc-multiple-inheritance)
class RootDecl: public Decl, public DeclContext {
public:
  static bool classof(const Decl *D) { return D->getKind() == Root; }

  explicit RootDecl(const SourceLocation L)
    : Decl(Root, L) {}
};

class NamedDecl : public Decl {
public:
  static bool classof(const Decl *D) { return D->getKind() >= firstNamed && D->getKind() <= lastNamed; }

protected:
  NamedDecl(const Kind K, const SourceLocation L) : Decl(K, L) {}
};

// NOLINTNEXTLINE(misc-multiple-inheritance)
class NamespaceDecl: public NamedDecl, public DeclContext {
public:
  static bool classof(const Decl *D) { return D->getKind() == Namespace; }

  explicit NamespaceDecl(const SourceLocation L)
    : NamedDecl(Namespace, L) {}

protected:
};

class ChannelDecl: public NamedDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Channel; }

  explicit ChannelDecl(const SourceLocation L)
    : NamedDecl(Channel, L) {}
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

  explicit ComponentDecl(const SourceLocation L) : RecordDecl(Component, L) {}
};

class TraitDecl : public RecordDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Trait; }

  explicit TraitDecl(const SourceLocation L) : RecordDecl(Trait, L) {}
};

class ImplDecl: public RecordDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Impl; }

  explicit ImplDecl(const SourceLocation L) : RecordDecl(Impl, L) {}
};

class ValueDecl : public NamedDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() >= firstValue && D->getKind() <= lastValue; }

protected:
  using NamedDecl::NamedDecl;
};

class FieldDecl : public ValueDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Field; }

  explicit FieldDecl(const SourceLocation L) : ValueDecl(Field, L) {}
};

class VarDecl : public ValueDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == Var; }

  explicit VarDecl(const SourceLocation L) : ValueDecl(Var, L) {}
};

class ParmVarDecl : public ValueDecl {
public:
  static bool classof(const Decl *D) { return D->getKind() == ParmVar; }

  explicit ParmVarDecl(const SourceLocation L) : ValueDecl(ParmVar, L) {}
};


} // namespace gstrands