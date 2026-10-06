#pragma once
#include "gstrands/AST/DeclBase.h"
#include "gstrands/Vocab/TokenDefs.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringTable.h"

namespace gstrands {

// Needed by string helper declarations emitted by this backend.
using llvm::StringRef;
using llvm::StringTable;

// Generate enums before the structs that use them.
#define GET_DeclTerminationKind_DECL
#define GET_GrammarContextKind_DECL
#include "gstrands/Parse/ParserPolicy.inc"

struct DeclSyntaxInfo {
  tok::TokenKind Starter;
  Decl::Kind Node;
  DeclTerminationKind TerminationPolicy;
};

struct DeclNestingPolicy {
  GrammarContextKind Context;
  tok::TokenKind Starter;
};

// Generate lookup declarations after their return types exist.
#define GET_DeclSyntaxTable_DECL
#define GET_DeclNestingTable_DECL
#include "gstrands/Parse/ParserPolicy.inc"

} // namespace gstrands
