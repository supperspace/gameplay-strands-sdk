#include "gstrands/Frontend/Parse/Parser.h"

#include "gstrands/Frontend/AST/ASTContext.h"
#include "gstrands/Frontend/AST/Decl.h"
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
  constexpr ParserFrame RootFrame {
    .AllowsNamespace = true,
    .AllowsTypeDecl = true,
    .AllowsChannel = true
  };

  const ScopedParserFrame F(this, RootFrame);
  auto Decls = parseDecls(tok::EndOfFile);

  Context.getRootDecl()->setChildren(Context.copyArray(llvm::ArrayRef(Decls)));
}

Decl *Parser::parseDecl() {
  const auto& CurrentFrame = ParserFrameStack.top();

  const auto Tok = Tokens.lookAhead();
  switch (Tok.Kind) {
  case tok::KwNamespace:
    if (CurrentFrame.AllowsNamespace) {
      return parseNamespaceDecl();
    }
    Diag.report(diag::DeclNotAllowed, Tok.SourceRange.getStartLoc());
    break;
  case tok::KwComponent:
    if (CurrentFrame.AllowsTypeDecl) {
      return parseComponentDecl();
    }
    Diag.report(diag::DeclNotAllowed, Tok.SourceRange.getStartLoc());
    break;
  case tok::KwTrait:
    if (CurrentFrame.AllowsTypeDecl) {
      return parseTraitDecl();
    }
    Diag.report(diag::DeclNotAllowed, Tok.SourceRange.getStartLoc());
    break;
  case tok::KwImpl:
    if (CurrentFrame.AllowsTypeDecl) {
      return parseImplDecl();
    }
    Diag.report(diag::DeclNotAllowed, Tok.SourceRange.getStartLoc());
    break;
  case tok::KwChannel:
    if (CurrentFrame.AllowsChannel) {
      return parseChannelDecl();
    }
    Diag.report(diag::DeclNotAllowed, Tok.SourceRange.getStartLoc());
  default: break;
  }

  // todo: We need to think about the right recovery strategy for this case
  Diag.report(diag::SyntaxError, Tok.SourceRange.getStartLoc());
  return nullptr;
}

llvm::SmallVector<Decl *, 16> Parser::parseDecls(const tok::TokenKind Until) {
  llvm::SmallVector<Decl *, 16> Result;

  auto Tok = Tokens.lookAhead();
  while (Tok.Kind != Until && Tok.Kind != tok::EndOfFile) {
    Decl* NewDecl = parseDecl();
    assert(NewDecl); // slapping an assertion until we decide how to hanle this case
    Result.push_back(NewDecl);

    Tok = Tokens.lookAhead();
  }

  return Result;
}

NamespaceDecl * Parser::parseNamespaceDecl() {
  SourceLocation StartLoc;
  if (const auto OptTok = Tokens.conditionalConsume(tok::KwNamespace); OptTok) {
    StartLoc = OptTok.value().SourceRange.getStartLoc();
    parseQualifiedName();
  }

  auto& NewNamespace = Context.emplace<NamespaceDecl>(StartLoc);

  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    // Parse the declarations within
    constexpr ParserFrame NamespaceParserFrame {
      .AllowsNamespace = true,
      .AllowsTypeDecl = true,
      .AllowsChannel = true
    };

    const ScopedParserFrame F(this, NamespaceParserFrame);

    const auto Children = parseDecls(tok::TokenKind::RBrace);
    NewNamespace.setChildren(Context.copyArray(llvm::ArrayRef(Children)));
  }

  if (const auto T = Tokens.conditionalConsume(tok::RBrace); T == std::nullopt) {
    // Cannot find matching brace
    Diag.report(diag::SyntaxError, Tokens.lookAhead().SourceRange.getStartLoc());
  }

  return &NewNamespace;
}

ComponentDecl* Parser::parseComponentDecl() {
  SourceLocation StartLoc;

  if (const auto OptTok = Tokens.conditionalConsume(tok::KwComponent); OptTok) {
    StartLoc = OptTok.value().SourceRange.getStartLoc();
    parseQualifiedName(); // This should be just a name, no qualifiers allowed
  }

  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    skipUntilEnclosingBrace(); // Just a temporary workaround so we can parse through the file
  }

  return &Context.emplace<ComponentDecl>(StartLoc);
}

TraitDecl* Parser::parseTraitDecl() {
  SourceLocation StartLoc;

  if (const auto OptTok = Tokens.conditionalConsume(tok::KwTrait); OptTok) {
    StartLoc = OptTok.value().SourceRange.getStartLoc();
    parseQualifiedName(); // This should be just a name, no qualifiers allowed
  }

  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    skipUntilEnclosingBrace(); // Just a temporary workaround so we can parse through the file
  }

  return &Context.emplace<TraitDecl>(StartLoc);
}

ImplDecl *Parser::parseImplDecl() {

  auto Tok = Tokens.consume();
  assert(Tok.Kind == tok::KwImpl);

  const SourceLocation StartLoc = Tok.SourceRange.getStartLoc();

  // Parse the trait name
  parseQualifiedName(); // This should be just a name, no qualifiers allowed

  // then we expect 'on'
  Tok = Tokens.lookAhead();
  if (Tok.Kind != tok::KwOn) {
    Diag.report(diag::SyntaxError, Tok.SourceRange);
  }
  else {
    // If we know it's 'on', we consume it. Otherwise we don't and we speculatively try to parse on
    Tokens.consume();
  }

  // Parse the component name
  parseQualifiedName();

  // Then we expect the opening brace so we can skip the rest of the body from for now
  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    skipUntilEnclosingBrace(); // Just a temporary workaround so we can parse through the file
  }

  return &Context.emplace<ImplDecl>(StartLoc);
}

ChannelDecl *Parser::parseChannelDecl() {
  SourceLocation StartLoc;

  if (const auto OptTok = Tokens.conditionalConsume(tok::KwChannel); OptTok) {
    StartLoc = OptTok.value().SourceRange.getStartLoc();

    // if (OptTok = Tokens.conditionalConsume(tok::KwChannel); OptTok) {
    //
    // }
  }
  skipUntil(tok::Semicolon);

  return &Context.emplace<ChannelDecl>(StartLoc);
}

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

void Parser::skipUntil(const tok::TokenKind K) {
  auto AheadK = Tokens.lookAhead().Kind;
  while (AheadK != K && AheadK != tok::EndOfFile) {
    Tokens.consume();
    AheadK = Tokens.lookAhead().Kind;
  }

  if (AheadK == K) {
    Tokens.consume();
  }
}

bool Parser::skipUntilEnclosingBrace() {
  auto AheadK = Tokens.lookAhead().Kind;
  size_t Counter = 0;
  while (true) {

    if (AheadK == tok::LBrace) {
      ++Counter;
    }
    else if (AheadK == tok::RBrace) {
      if (Counter == 0) {
        // we're about to early out before we consume the token, so consume it now
        Tokens.consume();
        return true;
      }

      --Counter;
    }
    else if (AheadK == tok::EndOfFile) {
      return false;
    }

    Tokens.consume();
    AheadK = Tokens.lookAhead().Kind;
  }

  return false;
}

} // namespace gstrands