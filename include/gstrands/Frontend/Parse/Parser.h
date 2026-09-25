#pragma once
#include "gstrands/Frontend/Lex/Token.h"

namespace gstrands {
class Lexer;

class TokenStream {
public:
  explicit TokenStream(Lexer& L) : Lex(L) {}

  /// consumes the next token
  Token consume();

  /// Peeks at the next token, but does not consume it.
  Token lookAhead();


private:
  Lexer& Lex;
  llvm::SmallVector<Token, 8> BufferedTokens;
};

class Parser {
  public:
    explicit Parser(Lexer& Lex)
      : Tokens(Lex) {}

    void parse();
  private:
    void parseTopLevelDecl();

    TokenStream Tokens;
  };

} // namespace gstrands