#include "gstrands/Frontend/Parse/Parser.h"

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

void Parser::parse() {

  auto Tok = Tokens.lookAhead();

  while (Tok.Kind != tok::EndOfFile) {
    parseTopLevelDecl();
    Tok = Tokens.lookAhead();
  }
}

void Parser::parseTopLevelDecl() {
  const auto &Tok = Tokens.lookAhead();
  switch (Tok.Kind) {
  case tok::KwNamespace: break;
  case tok::KwComponent: break;
  case tok::KwTrait: break;
    default: break;
  }


}

} // namespace gstrands