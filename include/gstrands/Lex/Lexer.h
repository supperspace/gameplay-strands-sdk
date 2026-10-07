#pragma once

#include "gstrands/Basic/SourceManager.h"
#include "gstrands/Lex/Token.h"

#include "llvm/Support/BLAKE3.h"

namespace gstrands {

class IdentifierTable;

class Lexer {

public:
  Lexer(SourceBufferView InSourceBufferView, IdentifierTable& InIdentifierTable);
  Token lex();

  template <size_t N>
  void takeFinalBlake3(std::array<uint8_t, N>& Out) {
    Blake3.final(Out);
  }

private:
  SourceBufferView SourceBuffView;
  IdentifierTable& IdentTable;

  llvm::StringRef Remaining;

  llvm::StringRef::value_type getNextChar();
  llvm::StringRef::value_type peekNextChar() const;

  void advance(size_t N = 1);

  void skipWhitespace();

  bool conditionalAdvance(llvm::StringRef Match);
  void skipUntilIncluding(llvm::StringRef Match);

  void skipUntilLineEnd();

  SourceLocation getCurrentSourceLoc() const;

  Token lexIdentifier();
  std::optional<Token> lexNumericLiteral();
  Token lexStringLiteral();
  Token lexCharacterLiteral();
  Token lexDelimitedLiteral(char Delimiter, tok::TokenKind TK);

  void digestToken(tok::TokenKind TK);

  llvm::BLAKE3 Blake3;
};

} // namespace gstrands