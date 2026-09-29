#pragma once

//=== ASTTableGen.h - Common definitions for AST node tablegen --*- C++ -*-===//
//
// Derived from clang/utils/TableGen/ASTTableGen.h,
// originally part of the LLVM Project.
//
// The upstream code is licensed under Apache License v2.0 with LLVM
// Exceptions. See LICENSES/LLVM.txt and https://llvm.org/LICENSE.txt.
//
// GameplayStrands modifications are licensed under Apache License v2.0.
// See LICENSE.
//
// Modified for GameplayStrands: adapted node generation and naming conventions.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/STLExtras.h"
#include "llvm/TableGen/Record.h"

// ASTNodes and their common fields.  `Base` is actually defined
// in subclasses, but it's still common across the hierarchies.
#define ASTNodeClassName "ASTNode"
#define BaseFieldName "Base"
#define AbstractFieldName "Abstract"

// Decl node hierarchy.
#define DeclNodeClassName "DeclNode"
#define DeclContextNodeClassName "DeclContext"

namespace gstrands::tblgen {

class WrappedRecord {
  const llvm::Record *Record;

protected:
  explicit(false) WrappedRecord(const llvm::Record *R = nullptr) : Record(R) {}

  const llvm::Record *get() const {
    assert(Record && "accessing null record");
    return Record;
  }

public:
  const llvm::Record *getRecord() const { return Record; }

  explicit operator bool() const { return Record != nullptr; }

  llvm::ArrayRef<llvm::SMLoc> getLoc() const { return get()->getLoc(); }

  /// Does the node inherit from the given TableGen class?
  bool isSubClassOf(const llvm::StringRef ClassName) const { return get()->isSubClassOf(ClassName); }

  template <class NodeClass> NodeClass getAs() const {
    return (isSubClassOf(NodeClass::getTableGenNodeClassName()) ? NodeClass(get()) : NodeClass());
  }

  friend bool operator<(const WrappedRecord LHS, const WrappedRecord RHS) {
    assert(LHS && RHS && "sorting null nodes");
    return LHS.get()->getName() < RHS.get()->getName();
  }
  friend bool operator>(const WrappedRecord LHS, const WrappedRecord RHS) { return RHS < LHS; }
  friend bool operator<=(const WrappedRecord LHS, const WrappedRecord RHS) { return !(RHS < LHS); }
  friend bool operator>=(const WrappedRecord LHS, const WrappedRecord RHS) { return !(LHS < RHS); }
  friend bool operator==(const WrappedRecord LHS, const WrappedRecord RHS) {
    // This should handle null nodes.
    return LHS.getRecord() == RHS.getRecord();
  }
  friend bool operator!=(const WrappedRecord LHS, const WrappedRecord RHS) { return !(LHS == RHS); }
};

/// An (optional) reference to a TableGen node representing a class
/// in one of GStrands's AST hierarchies.
class ASTNode : public WrappedRecord {
public:
  explicit(false) ASTNode(const llvm::Record *R = nullptr) : WrappedRecord(R) {}

  llvm::StringRef getName() const { return get()->getName(); }

  /// Return the node for the base, if there is one.
  ASTNode getBase() const { return get()->getValueAsOptionalDef(BaseFieldName); }

  /// Is the corresponding class abstract?
  bool isAbstract() const { return get()->getValueAsBit(AbstractFieldName); }

  static llvm::StringRef getTableGenNodeClassName() { return ASTNodeClassName; }
};

}// namespace gstrands::tblgen
