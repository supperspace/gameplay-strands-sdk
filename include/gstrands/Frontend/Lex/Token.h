#pragma once
#include "gstrands/Frontend/Basic/IdentifierTable.h"
#include "gstrands/Frontend/Basic/SourceLocation.h"

#include "TokenDefs.h"

namespace gstrands {

  struct Token {

    Token(const Identifier Ident, const SourceRange Range)
      : Kind(Ident.getTokenKind())
      , Ident(Ident)
      , SourceRange(Range)
      , Spelling(Ident.getSpelling()) {}

    Token(const tok::TokenKind Kind, const SourceRange Range)
      : Kind(Kind)
      , SourceRange(Range) {}

    Token() = default;

    tok::TokenKind Kind = tok::Invalid;
    Identifier Ident;

    /// The full range this token is defined at
    SourceRange SourceRange;
    
    llvm::StringRef Spelling;
  };
  
}// namespace gstrands