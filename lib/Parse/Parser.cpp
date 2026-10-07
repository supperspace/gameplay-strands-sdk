#include "gstrands/Parse/Parser.h"

#include "gstrands/AST/ASTContext.h"
#include "gstrands/AST/Decl.h"
#include "gstrands/Basic/Diagnostics.h"
#include "gstrands/Lex/Lexer.h"

namespace gstrands {

Token TokenStream::consume() {
  if (!BufferedTokens.empty()) {
    const auto Result = BufferedTokens.front();
    BufferedTokens.erase(BufferedTokens.begin());

    return Result;
  }

  return Lex.lex();
}

SourceRange TokenStream::consumeN(size_t N) {
  SourceRange Range;
  // First drop as many buffered tokens as possible
  const size_t BufferedDropN = std::min(BufferedTokens.size(), N);
  if (BufferedDropN > 0) {
    Range = BufferedTokens.front().SourceRange + BufferedTokens[BufferedDropN - 1].SourceRange;
    BufferedTokens.erase(BufferedTokens.begin(), BufferedTokens.begin() + BufferedDropN);
  }

  // drop tokens one by one without buffering
  N -= BufferedDropN;
  while (N > 0) {

    if (const auto T = Lex.lex(); T.Kind == tok::EndOfFile)
      return Range;
    else
      Range = Range + T.SourceRange;
    --N;
  }

  return Range;
}

Token TokenStream::lookAhead(const size_t N) {
  if (N >= BufferedTokens.size()) {
    ssize_t MissingTokenCount = static_cast<ssize_t>((N + 1) - BufferedTokens.size());
    do {
      BufferedTokens.push_back(Lex.lex());
      --MissingTokenCount;
    }
    while (MissingTokenCount > 0);
  }

  return BufferedTokens[N];
}

std::optional<Token> TokenStream::conditionalConsume(tok::TokenKind K) {
  if (const auto T = lookAhead(); T.Kind == K) {
    return consume();
  }
  return std::nullopt;
}

void Parser::parse() {
  const ParserFrame RootFrame {
    .ParentContext = Context.getRootDecl(),
    .ContextKind = RootGrammar
  };

  const ScopedParserFrame F(this, RootFrame);
  auto Decls = parseDecls(tok::EndOfFile);

  Context.getRootDecl()->setChildren(Context.copyArray(llvm::ArrayRef(Decls)));
}

Decl *Parser::parseDecl() {
  const auto Tok = Tokens.lookAhead();

  assert(startsDeclaration(Tok.Kind));
  const bool IsDeclarationAccepted = acceptsDeclaration(Tok.Kind);

  Decl *ParsedDecl = nullptr;
  switch (Tok.Kind) {
  case tok::KwStruct:
    llvm_unreachable("not yet implemented");
    break;
  case tok::KwTrait:
    ParsedDecl = parseTraitDecl();
    break;
  case tok::KwDelegate:
    llvm_unreachable("not yet implemented");
    break;
  case tok::KwNamespace:
    ParsedDecl = parseNamespaceDecl();
    break;
  case tok::KwComponent:
    ParsedDecl = parseComponentDecl();
    break;
  case tok::KwLink:
    llvm_unreachable("not yet implemented");
    break;
  case tok::KwImpl:
    ParsedDecl = parseImplDecl();
    break;
  case tok::KwChannel:
    ParsedDecl = parseChannelDecl();
    break;
  case tok::KwDef:
    llvm_unreachable("not yet implemented");
    break;
  case tok::KwProperty:
    llvm_unreachable("not yet implemented");
    break;
  default:
    llvm_unreachable("All declaration starters should be handled by explicit clauses.");
    break;
  }

  if (!IsDeclarationAccepted) {
    Diag.report(diag::DeclNotAllowed, Tok.SourceRange.getStartLoc()) << "Declaration not allowed";
  }
  else {
    ParsedDecl->setParent(getCurrentParseFrame().ParentContext);
  }

  return ParsedDecl;
}

llvm::SmallVector<Decl *, 16> Parser::parseDecls(const tok::TokenKind Until) {
  llvm::SmallVector<Decl *, 16> Result;
  auto Tok = Tokens.lookAhead();

  while (Tok.Kind != Until && Tok.Kind != tok::EndOfFile) {
    size_t Distance = 0;
    auto InitialLoc = Tok.SourceRange.getStartLoc();
    while (!startsDeclaration(Tok.Kind) && Tok.Kind != tok::EndOfFile && Tok.Kind != Until) {
      // start consuming
      Tok = Tokens.lookAhead(++Distance);
    }

    if (Distance > 0) {
      Diag.report(diag::ParseError, InitialLoc)
          << "encountered " << Distance << " unexpected tokens starting at location";
      Tokens.consumeN(Distance);
    }

    if (Tok.Kind == Until || Tok.Kind == tok::EndOfFile) {
      break;
    }

    if (Decl* NewDecl = parseDecl(); NewDecl) {
      Result.push_back(NewDecl);
    }

    Tok = Tokens.lookAhead();
  }

  return Result;
}

NamespaceDecl * Parser::parseNamespaceDecl() {
  auto Tok = Tokens.consume();
  assert(Tok.Kind == tok::KwNamespace);
  const SourceLocation StartLoc = Tok.SourceRange.getStartLoc();

  const auto QualifiedName = parseQualifiedName();
  assert(!QualifiedName.empty());

  auto &NewNamespace = Context.emplace<NamespaceDecl>(StartLoc, Context.copyArray(llvm::ArrayRef(QualifiedName)));

  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    // Parse the declarations within
    const ParserFrame NamespaceParserFrame {
      .ParentContext = &NewNamespace,
      .ContextKind = NamespaceGrammar
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
  auto Tok = Tokens.consume();
  assert(Tok.Kind == tok::KwComponent);
  const SourceLocation StartLoc = Tok.SourceRange.getStartLoc();

  Tok = Tokens.lookAhead();
  if (Tok.Kind != tok::Identifier) {
    // todo emit diag. Should we also insert some other identifier and continue parsing here, or how should we recover?
  }
  else {
    Tokens.consume();
  }

  const Identifier Ident = Tok.Ident;

  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    skipUntilEnclosingBrace(); // Just a temporary workaround so we can parse through the file
  }

  return &Context.emplace<ComponentDecl>(StartLoc, Ident);
}

TraitDecl* Parser::parseTraitDecl() {
  auto Tok = Tokens.consume();
  assert(Tok.Kind == tok::KwTrait);
  const SourceLocation StartLoc = Tok.SourceRange.getStartLoc();

  auto ParseBody = [this, StartLoc](const size_t StartingDepth = 0) {
    // temporary cop out
    if (!skipUntilEnclosingBrace(StartingDepth)) {
      Diag.report(diag::SyntaxError, StartLoc) << "trait is missing enclosing '}'";
    }
  };

  auto AttemptRecovery = [this, ParseBody]() {
    size_t Distance = 0;
    while (true) {
      const Token T = Tokens.lookAhead(Distance++);
      if (T.Kind == tok::LBrace) {
        // First throw away all the tokens we had to skip. todo turn this into useful diagnostic information
        Tokens.consumeN(Distance);

        // We can continue parsing the trait body from here
        ParseBody();
        return;
      }

      if (startsDeclaration(T.Kind)) {
        Tokens.consumeN(Distance - 1);
        if (acceptsDeclaration(TraitGrammar, T.Kind)) {
          ParseBody();
          return;
        }
        // we're done parsing this trait, we leave things here for the parent scope to pick up
        return;
      }

      if (T.Kind == tok::RBrace) {
        Tokens.consumeN(Distance-1);
        return;
      }

      if (T.Kind == tok::EndOfFile) {
        Tokens.consumeN(Distance - 1);
        return;
      }
    }
  };

  Tok = Tokens.lookAhead();
  Identifier Ident;
  const bool IsMissingIdentifier = Tok.Kind != tok::Identifier;
  if (IsMissingIdentifier) {
    Ident = Identifiers.get("**error**");
    Diag.report(diag::SyntaxError, Tok.SourceRange.getStartLoc()) << "expected an identifier to name the trait with";
  }
  else {
    Ident = Tok.Ident;
    Tokens.consume();
  }

  auto& Trait = Context.emplace<TraitDecl>(StartLoc, Ident);
  if (IsMissingIdentifier) {
    AttemptRecovery();
    return &Trait;
  }

  // If we're here, then the grammar was correct up to this point
  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    ParseBody();
  }
  else {
    Diag.report(diag::SyntaxError, Tokens.lookAhead().SourceRange.getStartLoc()) << "expected an opening '{' while parsing trait";
    AttemptRecovery();
  }

  return &Trait;
}

ImplDecl *Parser::parseImplDecl() {

  auto Tok = Tokens.consume();
  assert(Tok.Kind == tok::KwImpl);

  const SourceLocation StartLoc = Tok.SourceRange.getStartLoc();

  // Parse the trait name
  const auto TraitName = parseQualifiedName(); // This should be just a name, no qualifiers allowed

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
  const auto ImplementerName = parseQualifiedName();

  // Then we expect the opening brace so we can skip the rest of the body from for now
  if (const auto T = Tokens.conditionalConsume(tok::LBrace); T != std::nullopt) {
    skipUntilEnclosingBrace(); // Just a temporary workaround so we can parse through the file
  }

  return &Context.emplace<ImplDecl>(StartLoc, Context.copyArray(llvm::ArrayRef(TraitName)),
                                    Context.copyArray(llvm::ArrayRef(ImplementerName)));
}

ChannelDecl *Parser::parseChannelDecl() {
  auto Tok = Tokens.consume();
  assert(Tok.Kind == tok::KwChannel);
  const SourceLocation StartLoc = Tok.SourceRange.getStartLoc();

  Tok = Tokens.lookAhead();
  if (Tok.Kind != tok::Identifier) {
    // todo emit diag. Should we also insert some other identifier and continue parsing here, or how should we recover?
  }
  else {
    Tokens.consume();
  }

  const Identifier Ident = Tok.Ident;

  skipUntil(tok::Semicolon);
  return &Context.emplace<ChannelDecl>(StartLoc, Ident);
}

llvm::SmallVector<Identifier, 4> Parser::parseQualifiedName() {
  llvm::SmallVector<Identifier, 4> Result;

  // a Qualified name has at least an identifier, so expect an identifier
  auto Tok = Tokens.lookAhead();
  if (Tok.Kind != tok::Identifier) {
    // we have a problem
    Diag.report(diag::ParseError, Tok.SourceRange.getStartLoc());
  }
  else {
    Result.push_back(Tok.Ident);
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
    else {
      Result.push_back(Tok.Ident);
    }
    Tokens.consume();
    Tok = Tokens.lookAhead();
  }

  return Result;
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

bool Parser::skipUntilEnclosingBrace(size_t Depth) {
  auto AheadK = Tokens.lookAhead().Kind;

  while (true) {

    if (AheadK == tok::LBrace) {
      ++Depth;
    }
    else if (AheadK == tok::RBrace) {
      if (Depth == 0) {
        // we're about to early out before we consume the token, so consume it now
        Tokens.consume();
        return true;
      }

      --Depth;
    }
    else if (AheadK == tok::EndOfFile) {
      return false;
    }

    Tokens.consume();
    AheadK = Tokens.lookAhead().Kind;
  }

  return false;
}

std::optional<size_t> Parser::distanceTo(const tok::TokenKind K) {
  size_t Distance = 0;
  auto AheadK = Tokens.lookAhead(Distance).Kind;
  while (AheadK != K) {
    if (AheadK == tok::EndOfFile)
      return std::nullopt;

    AheadK = Tokens.lookAhead(++Distance).Kind;
  }

  return Distance;
}

std::optional<size_t> Parser::distanceToEnclosingBrace() {
  size_t Distance = 0;

  auto AheadK = Tokens.lookAhead().Kind;
  size_t Counter = 0;
  while (true) {

    if (AheadK == tok::LBrace)
      ++Counter;
    else if (AheadK == tok::RBrace) {
      if (Counter == 0)
        return Distance;
      --Counter;
    }
    else if (AheadK == tok::EndOfFile)
      return std::nullopt;

    AheadK = Tokens.lookAhead(++Distance).Kind;
  }
}

bool Parser::startsDeclaration(const tok::TokenKind K) const {
  return tok::lookupDeclKeyword(K) != nullptr;
}

bool Parser::acceptsDeclaration(const tok::TokenKind K) const {
  return lookupDeclNesting(getCurrentParseFrame().ContextKind, K) != nullptr;
}

bool Parser::acceptsDeclaration(const GrammarContextKind ContextK, const tok::TokenKind K) const {
  return lookupDeclNesting(ContextK, K) != nullptr;
}

} // namespace gstrands