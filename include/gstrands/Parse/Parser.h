#pragma once

#include "gstrands/Lex/Token.h"
#include "gstrands/AST/DeclFwd.h"

#include <stack>

namespace gstrands {
class DiagnosticsEngine;
class Lexer;
class ASTContext;

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
  explicit Parser(Lexer &Lex, DiagnosticsEngine &DE, ASTContext& AST) : Tokens(Lex), Diag(DE), Context(AST) {}

  void parse();

private:


  struct ParserFrame {
    uint16_t AllowsNamespace: 1 = false;
    uint16_t AllowsTypeDecl: 1  = false;
    uint16_t AllowsChannel: 1   = false;
  };

  struct ScopedParserFrame {
    [[nodiscard]] ScopedParserFrame(Parser* P, const ParserFrame F)
      : P(*P) {
      P->ParserFrameStack.push(F);
    }

    ~ScopedParserFrame() {
      P.ParserFrameStack.pop();
    }

    ScopedParserFrame(const ScopedParserFrame&) = delete;
    ScopedParserFrame(ScopedParserFrame&&) = delete;
    ScopedParserFrame& operator=(const ScopedParserFrame&) = delete;
    ScopedParserFrame& operator=(ScopedParserFrame&&) = delete;

    Parser& P;
  };

  Decl* parseDecl();
  llvm::SmallVector<Decl*, 16> parseDecls(tok::TokenKind Until);

  NamespaceDecl* parseNamespaceDecl();
  ComponentDecl* parseComponentDecl();
  TraitDecl* parseTraitDecl();
  ImplDecl* parseImplDecl();
  ChannelDecl* parseChannelDecl();

  llvm::SmallVector<Identifier, 4> parseQualifiedName();

  void skipUntil(tok::TokenKind K);
  /// Skips until the next '}', and if additional '{' are encountered, it skips over their enclosing '}' too
  /// returns true if an enclosing brace was found before EndOfFile
  bool skipUntilEnclosingBrace();

  TokenStream Tokens;
  DiagnosticsEngine &Diag;
  ASTContext &Context;

  std::stack<ParserFrame> ParserFrameStack;
};

} // namespace gstrands