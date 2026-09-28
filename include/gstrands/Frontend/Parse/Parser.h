#pragma once
#include "gstrands/Frontend/Lex/Token.h"

namespace gstrands {
class DiagnosticsEngine;
class Lexer;

class TokenStream {
public:
  explicit TokenStream(Lexer &L) : Lex(L) {}

  /// consumes the next token
  Token consume();

  /// Peeks at the next token, but does not consume it.
  Token lookAhead();

  /// consumes the next token only if its kind == K
  std::optional<Token> conditionalConsume(tok::TokenKind K);

private:
  Lexer &Lex;
  llvm::SmallVector<Token, 8> BufferedTokens;
};

class Parser {
public:
  explicit Parser(Lexer &Lex, DiagnosticsEngine &DE) : Tokens(Lex), Diag(DE) {}

  void parse();

private:
  void parseTopLevelDecl();

  void parseNamespaceDecl();
  void parseComponentDecl();
  void parseTraitDecl();
  void parseMutationDecl();
  void parsePropertyDecl();

  void parseExpression();

  void parseStatement();

  void parseQualifiedName();

  TokenStream Tokens;
  DiagnosticsEngine &Diag;
};

} // namespace gstrands