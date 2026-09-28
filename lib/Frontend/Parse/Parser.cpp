#include "gstrands/Frontend/Parse/Parser.h"

#include "gstrands/Frontend/Basic/Diagnostics.h"
#include "gstrands/Frontend/Lex/Lexer.h"

namespace gstrands {

Token TokenStream::consume() {
  if (!BufferedTokens.empty()) {
    const auto Result = BufferedTokens.front();
    BufferedTokens.erase(BufferedTokens.begin());

    return Result;
  }

  return Lex.lex();
}

Token TokenStream::lookAhead() {
  if (BufferedTokens.empty()) {
    BufferedTokens.push_back(Lex.lex());
  }

  return BufferedTokens.front();
}

std::optional<Token> TokenStream::conditionalConsume(tok::TokenKind K) {
  if (const auto T = lookAhead(); T.Kind == K) {
    return consume();
  }
  return std::nullopt;
}

void Parser::parse() {

  auto Tok = Tokens.lookAhead();

  while (Tok.Kind != tok::EndOfFile) {
    parseTopLevelDecl();
    Tok = Tokens.lookAhead();
  }
}

void Parser::parseTopLevelDecl() {

  switch (const auto Tok = Tokens.lookAhead(); Tok.Kind) {
  case tok::KwNamespace:
    parseNamespaceDecl();
    break;
  case tok::KwComponent:
    parseComponentDecl();
    break;
  case tok::KwTrait:
    parseTraitDecl();
    break;
    default: break;
  }

}

void Parser::parseNamespaceDecl() {
  if (const auto OptTok = Tokens.conditionalConsume(tok::KwNamespace); OptTok) {
    parseQualifiedName();
  }

  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    // Parse the declarations within
  }

  if (const auto T = Tokens.conditionalConsume(tok::RBrace); T == std::nullopt) {
    // Cannot find matching brace
  }

}

void Parser::parseComponentDecl() {}

void Parser::parseTraitDecl() {}

void Parser::parseQualifiedName() {
  // a Qualified name has at least an identifier, so expect an identifier
  auto Tok = Tokens.lookAhead();
  if (Tok.Kind != tok::Identifier) {
    // we have a problem
    Diag.report(diag::ParseError, Tok.SourceRange.getStartLoc());
  }

  Tokens.consume();
  Tok = Tokens.lookAhead();

  // Every '::' we now encounter demands an identifier to follow
  while (Tok.Kind == tok::ColonColon) {
    Tokens.consume();
    Tok = Tokens.lookAhead();
    if (Tok.Kind != tok::Identifier) {
      // once again, we're in trouble
    }
    Tokens.consume();
    Tok = Tokens.lookAhead();
  }
}

} // namespace gstrands