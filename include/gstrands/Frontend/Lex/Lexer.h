#pragma once

#include "gstrands/Frontend/Basic/SourceManager.h"

#include "Token.h"

namespace gstrands {

class IdentifierTable;

class Lexer {

public:
  Lexer(const SourceBufferView InSourceBufferView, IdentifierTable& InIdentifierTable);
  Token lex();

private:
  SourceBufferView SourceBuffView;
  IdentifierTable& IdentTable;

  llvm::StringRef Remaining;

  llvm::StringRef::value_type getNextChar();
  llvm::StringRef::value_type peekNextChar() const;

  void skipWhitespace();

  bool conditionalAdvance(llvm::StringRef Match);
  void skipUntilIncluding(llvm::StringRef Match);

  void skipUntilLineEnd();

  SourceLocation getCurrentSourceLoc() const;

  Token lexIdentifier();
  Token lexNumericLiteral();
};

} // namespace gstrands