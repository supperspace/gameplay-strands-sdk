#pragma once

#include "gstrands/Parse/ParserPolicy.h"
#include "gstrands/Lex/Token.h"
#include "gstrands/AST/DeclFwd.h"

#include <stack>

namespace gstrands {
class DeclContext;
class DiagnosticsEngine;
class Lexer;
class ASTContext;

class TokenStream {
public:
  explicit TokenStream(Lexer &L) : Lex(L) {}

  /// consumes the next token
  Token consume();

  SourceRange consumeN(size_t N);

  /// Peeks ahead N tokens, leaving the token and all the preceding ones in the buffer. If peeking pas the end of file
  /// token, end of file is returned instead (@safe)
  Token lookAhead(size_t N = 0);

  /// consumes the next token only if its kind == K
  std::optional<Token> conditionalConsume(tok::TokenKind K);

private:
  Lexer &Lex;
  llvm::SmallVector<Token, 8> BufferedTokens;
};

class Parser {
public:
  Parser(Lexer &Lex, DiagnosticsEngine &DE, ASTContext &AST, IdentifierTable &I)
      : Tokens(Lex), Diag(DE), Context(AST), Identifiers(I) {}

  void parse();

private:

  struct ParserFrame {
    DeclContext* ParentContext = nullptr;
    GrammarContextKind ContextKind;
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
  bool skipUntilEnclosingBrace(size_t Depth = 0);

  /// Returns the distance to the next token of the given kind or nullopt when end of file is found first
  std::optional<size_t> distanceTo(tok::TokenKind K);
  /// Returns the distance to the next enclosing brace, handling nesting or to the end of file if no brace is found
  std::optional<size_t> distanceToEnclosingBrace();

  bool startsDeclaration(tok::TokenKind K) const;
  /// Returns true if the current parse context accepts a declaration started by the provided token kind
  bool acceptsDeclaration(tok::TokenKind K) const;
  bool acceptsDeclaration(GrammarContextKind ContextK, tok::TokenKind K) const;

  const ParserFrame& getCurrentParseFrame() const {
    return ParserFrameStack.top();
  }

  TokenStream Tokens;
  DiagnosticsEngine &Diag;
  ASTContext &Context;
  IdentifierTable& Identifiers;

  std::stack<ParserFrame> ParserFrameStack;
};

} // namespace gstrands