#include "gstrands/Frontend/Lex/Lexer.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Error.h"

namespace gstrands {

Lexer::Lexer(const SourceBufferView InSourceBufferView, IdentifierTable& InIdentifierTable)
  : SourceBuffView(InSourceBufferView)
  , IdentTable(InIdentifierTable)
  , Remaining(SourceBuffView.BufferRef.getBuffer())
{}

Token Lexer::lex() {

  // skip whitespace and comments
  bool DidSkip = false;

  do {
    skipWhitespace();
    if (conditionalAdvance("//")) {
      // "//" comment, skip till end of line
      skipUntilLineEnd();
      DidSkip = true;
    }

    if (conditionalAdvance("/*")) {
      // "/*" comment, skip till enclosing "*/"
      skipUntilIncluding("*/");
      DidSkip = true;
    }

  } while (DidSkip);

  // Now we're ready to look for some actual tokens
  auto C  = peekNextChar();
  switch (C) {
  case '!': break;
  case '\"': break;
  case '#': break;
  case '$': break;
    case '%': break;
    case '&': break;
    case '\'': break;
    case '(': break;
    case ')': break;
    case '*': break;
    case '+': break;
    case ',': break;
    case '-': break;

  case '.': break; // Could be a float literal or a plain dot token
    case '/': break;
    // Digits may start number literals
    case '0': [[fallthrough]];
    case '1': [[fallthrough]];
    case '2': [[fallthrough]];
    case '3': [[fallthrough]];
    case '4': [[fallthrough]];
    case '5': [[fallthrough]];
    case '6': [[fallthrough]];
    case '7': [[fallthrough]];
    case '8': [[fallthrough]];
    case '9':
      return lexNumericLiteral();
  case ':': break;
    case ';': break;
  case '<': break;
  case '=': break;
    case '>': break;
    case '?': break;
  case '@': break;
    // Identifiers may be started by [a-zA-Z_]
  case 'A': [[fallthrough]];
  case 'B': [[fallthrough]];
  case 'C': [[fallthrough]];
  case 'D': [[fallthrough]];
  case 'E': [[fallthrough]];
  case 'F': [[fallthrough]];
  case 'G': [[fallthrough]];
  case 'H': [[fallthrough]];
  case 'I': [[fallthrough]];
  case 'J': [[fallthrough]];
  case 'K': [[fallthrough]];
  case 'L': [[fallthrough]];
  case 'M': [[fallthrough]];
  case 'N': [[fallthrough]];
  case 'O': [[fallthrough]];
  case 'P': [[fallthrough]];
  case 'Q': [[fallthrough]];
  case 'R': [[fallthrough]];
  case 'S': [[fallthrough]];
  case 'T': [[fallthrough]];
  case 'U': [[fallthrough]];
  case 'V': [[fallthrough]];
  case 'W': [[fallthrough]];
  case 'X': [[fallthrough]];
  case 'Y': [[fallthrough]];
  case 'Z': return lexIdentifier();

  case '[': break;
  case '\\': break;
  case  ']': break;
  case '^': break;
  case '_': return lexIdentifier();
  case '`': break;

  case 'a': [[fallthrough]];
    case 'b': [[fallthrough]];
    case 'c': [[fallthrough]];
    case 'd': [[fallthrough]];
    case 'e': [[fallthrough]];
    case 'f': [[fallthrough]];
    case 'g': [[fallthrough]];
    case 'h': [[fallthrough]];
    case 'i': [[fallthrough]];
    case 'j': [[fallthrough]];
    case 'k': [[fallthrough]];
    case 'l': [[fallthrough]];
    case 'm': [[fallthrough]];
    case 'n': [[fallthrough]];
    case 'o': [[fallthrough]];
    case 'p': [[fallthrough]];
    case 'q': [[fallthrough]];
    case 'r': [[fallthrough]];
    case 's': [[fallthrough]];
    case 't': [[fallthrough]];
    case 'u': [[fallthrough]];
    case 'v': [[fallthrough]];
    case 'w': [[fallthrough]];
    case 'x': [[fallthrough]];
    case 'y': [[fallthrough]];
    case 'z': return lexIdentifier();

    case '{': break;
    case '|': break;
  case '}': break;
  case '~': break;
  }


  return {};
}

llvm::StringRef::value_type Lexer::getNextChar() {
  const auto Res = Remaining.front();
  Remaining = Remaining.drop_front();
  return Res;
}

llvm::StringRef::value_type Lexer::peekNextChar() const {
  return Remaining.front();
}

void Lexer::skipWhitespace() { Remaining = Remaining.ltrim(); }

bool Lexer::conditionalAdvance(llvm::StringRef Match) {
  if (Remaining.starts_with(Match)) {
    Remaining = Remaining.drop_front(Match.size());
    return true;
  }

  return false;
}

void Lexer::skipUntilIncluding(llvm::StringRef Match) {
  const size_t End = Remaining.find(Match);

  if (End == llvm::StringRef::npos) {
    // We just scanned to the end of the file and didn't fine the expected
    // match. diag
    Remaining = Remaining.drop_front(Remaining.size());
  } else {
    Remaining = Remaining.drop_front(Match.size());
  }
}

void Lexer::skipUntilLineEnd() {

  Remaining = Remaining.drop_until(
      [](const char C) -> bool { return C == '\n' || C == '\r'; });
}

SourceLocation Lexer::getCurrentSourceLoc() const {
  const size_t Pos = Remaining.data() - SourceBuffView.BufferRef.getBufferStart();
  return SourceBuffView.BaseLocation + Pos;
}

Token Lexer::lexIdentifier() {
  const SourceLocation StartLoc = getCurrentSourceLoc();

  const auto Spelling = Remaining.take_while(
      [](auto C) -> bool { return llvm::isAlnum(C) || C == '_'; });

  Remaining = Remaining.drop_front(Spelling.size());

  SourceLocation EndLoc = getCurrentSourceLoc();

  auto Ident = IdentTable.get(Spelling);
  return {Ident, {StartLoc, EndLoc}};
}

Token Lexer::lexNumericLiteral() {
  return {};
}

} // namespace gstrands