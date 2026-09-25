#include "gstrands/Frontend/Lex/Lexer.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Error.h"

namespace gstrands {

Lexer::Lexer(const SourceBufferView InSourceBufferView,
             IdentifierTable &InIdentifierTable)
    : SourceBuffView(InSourceBufferView), IdentTable(InIdentifierTable),
      Remaining(SourceBuffView.BufferRef.getBuffer()) {}

Token Lexer::lex() {

  // skip whitespace and comments
  bool HadToSkipComment = false;

  do {
    HadToSkipComment = false;
    skipWhitespace();
    if (conditionalAdvance("//")) {
      // "//" comment, skip till end of line
      skipUntilLineEnd();
      HadToSkipComment = true;
    }

    if (conditionalAdvance("/*")) {
      // "/*" comment, skip till enclosing "*/"
      skipUntilIncluding("*/");
      HadToSkipComment = true;
    }

  } while (HadToSkipComment);

  // Now we're ready to look for some actual tokens
  const auto StartLoc = getCurrentSourceLoc();
  if (Remaining.empty()) {
    return Token(tok::EndOfFile, {StartLoc, StartLoc});
  }

  auto C = peekNextChar();

  auto MakeToken = [this, StartLoc](const tok::TokenKind K) -> Token {
    return Token{K, {StartLoc, getCurrentSourceLoc()}};
  };

  auto MakeTokenAdvance = [this, MakeToken](const tok::TokenKind K,
                                            const size_t N = 1) -> Token {
    advance(N);
    return MakeToken(K);
  };

  switch (C) {
  case '!':
    if (conditionalAdvance("!="))
      return MakeToken(tok::ExclaimEqual);

    return MakeTokenAdvance(tok::Exclaim);

  case '\"':
    llvm_unreachable("Language has no string literals _yet_");
    break;
  case '#':
    llvm_unreachable("Language has no tokens containing '#'");
    break;
  case '$':
    llvm_unreachable("Language has no tokens containing '$'");
    break;
  case '%':
    if (conditionalAdvance("%="))
      return MakeToken(tok::PercentEqual);
    return MakeTokenAdvance(tok::Percent);
  case '&':
    if (conditionalAdvance("&="))
      return MakeToken(tok::AmpEqual);
    if (conditionalAdvance("&&"))
      return MakeToken(tok::AmpAmp);
    return MakeTokenAdvance(tok::Amp);

  case '\'':
    llvm_unreachable("Language has no character literal tokens");
    break;
  case '(':
    return MakeTokenAdvance(tok::LParen);
  case ')':
    return MakeTokenAdvance(tok::RParen);
  case '*':
    if (conditionalAdvance("*="))
      return MakeToken(tok::StarEqual);
    return MakeTokenAdvance(tok::Star);

  case '+':
    if (conditionalAdvance("+="))
      return MakeToken(tok::PlusEqual);
    if (conditionalAdvance("++"))
      return MakeToken(tok::PlusPlus);

    return MakeTokenAdvance(tok::Plus);
  case ',':
    return MakeTokenAdvance(tok::Comma);

  case '-':
    if (conditionalAdvance("-=")) {
      return MakeToken(tok::MinusEqual);
    }
    if (conditionalAdvance("--")) {
      return MakeToken(tok::MinusMinus);
    }
    if (conditionalAdvance("->")) {
      return MakeToken(tok::Arrow);
    }

    return MakeTokenAdvance(tok::Minus);

  case '.':
    if (const auto OptTok = lexNumericLiteral(); OptTok.has_value()) {
      return OptTok.value();
    }
    return MakeTokenAdvance(tok::Period);
  case '/':
    if (conditionalAdvance("/="))
      return MakeToken(tok::SlashEqual);
    return MakeTokenAdvance(tok::Slash);

    // Digits may start number literals
    // clang-format off

  case '0': [[fallthrough]];
  case '1': [[fallthrough]];
  case '2': [[fallthrough]];
  case '3': [[fallthrough]];
  case '4': [[fallthrough]];
  case '5': [[fallthrough]];
  case '6': [[fallthrough]];
  case '7': [[fallthrough]];
  case '8': [[fallthrough]];
    // clang-format on
  case '9':
    return lexNumericLiteral().value();
  case ':':
    if (conditionalAdvance("::"))
      return MakeToken(tok::ColonColon);
    return MakeTokenAdvance(tok::Colon);
  case ';':
    return MakeTokenAdvance(tok::Semicolon);
  case '<':
    if (conditionalAdvance("<="))
      return MakeToken(tok::LessEqual);
    return MakeTokenAdvance(tok::Less);

  case '=':
    if (conditionalAdvance("=="))
      return MakeToken(tok::EqualEqual);
    return MakeTokenAdvance(tok::Equal);

  case '>':
    if (conditionalAdvance(">="))
      return MakeToken(tok::GreaterEqual);
    return MakeTokenAdvance(tok::Greater);

  case '?':
    llvm_unreachable("Language has no tokens containing '?'");
    break;
  case '@':
    return MakeTokenAdvance(tok::At);

    // Identifiers may be started by [a-zA-Z_]
    // clang-format off
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
    // clang-format on
  case 'Z':
    return lexIdentifier();

  case '[':
    return MakeTokenAdvance(tok::LSquare);

  case '\\':
    llvm_unreachable("Language has no tokens containing '\\'");
    break;
  case ']':
    return MakeTokenAdvance(tok::RSquare);
  case '^':
    if (conditionalAdvance("^="))
      return MakeToken(tok::CaretEqual);
    return MakeTokenAdvance(tok::Caret);
  case '_':
    return lexIdentifier();
  case '`':
    llvm_unreachable("Language has no tokens containing '`'");
    break;

    // clang-format off
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
    // clang-format on
  case 'z':
    return lexIdentifier();

  case '{':
    return MakeTokenAdvance(tok::LBrace);
  case '|':
    if (conditionalAdvance("|="))
      return MakeToken(tok::PipeEqual);
    if (conditionalAdvance("||"))
      return MakeToken(tok::PipePipe);
    return MakeTokenAdvance(tok::Pipe);
  case '}':
    return MakeTokenAdvance(tok::RBrace);
  case '~':
    return MakeTokenAdvance(tok::Tilde);
  default:
    llvm_unreachable("Invalid character");
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

void Lexer::advance(const size_t N) { Remaining = Remaining.drop_front(N); }

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

    Remaining = Remaining.drop_front(End + Match.size());
  }
}

void Lexer::skipUntilLineEnd() {
  // First we skip until we find the first line ending character
  Remaining = Remaining.drop_until(
      [](const char C) -> bool { return C == '\n' || C == '\r'; });

  skipWhitespace();
}

SourceLocation Lexer::getCurrentSourceLoc() const {
  const size_t Pos =
      Remaining.data() - SourceBuffView.BufferRef.getBufferStart();
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

std::optional<Token> Lexer::lexNumericLiteral() {
  // Important qualification: this function only handles numbers in base 10
  // That's not a language design decision, rather just a temporary deferral

  const llvm::StringRef StartingBuff = Remaining;
  if (Remaining.empty()) {
    return std::nullopt;
  }
  const auto StartLoc = getCurrentSourceLoc();

  // Consume decimal part
  const bool FirstDigitIs0 = Remaining.front() == '0';
  bool StartsWithDigits = FirstDigitIs0;
  while (!Remaining.empty() && Remaining.front() >= '0' &&
         Remaining.front() <= '9') {
    Remaining = Remaining.drop_front(1);
    StartsWithDigits = true;
  }

  const bool HasDot = !Remaining.empty() && Remaining.front() == '.';
  // If a number does not start with digits, but starts with a '.', then it must
  // have fractional digits. eg: .3 is a valid floating point number: .3 == 0.3

  const bool RequiresFractionalDigits = HasDot && !StartsWithDigits;
  bool IsMissingRequiredFractionalDigits = false;
  bool HasFractionalDigits = false;
  if (HasDot) {
    // consume the . and then consume an optional fractional part
    Remaining = Remaining.drop_front(1);

    if (!Remaining.empty() && Remaining.front() >= '0' && Remaining.front() <= '9') {
      HasFractionalDigits = true;
      while (!Remaining.empty() && Remaining.front() >= '0' &&
             Remaining.front() <= '9') {
        Remaining = Remaining.drop_front(1);
      }
    } else {
      // We have a dot, and the token did not start with a digit, nor does a
      // digit follow after the dot. This does not seem to be a number literal
      // But keep scanning, in case we encounter other number like artifacts, so
      // we can report on a malformed number literal
      IsMissingRequiredFractionalDigits = RequiresFractionalDigits;
    }
  }

  bool HasExponent = false;
  bool IsMissingRequiredExponentDigits = false;

  // Next we look for an exponent.
  if (!Remaining.empty() && (Remaining.front() == 'e' ||
      Remaining.front() == 'E')) {
    HasExponent = true;
    IsMissingRequiredExponentDigits = true;

    // consume the e, then require exponent digits after an optional sign
    Remaining = Remaining.drop_front(1);

    if (!Remaining.empty() && (Remaining.front() == '+' ||
        Remaining.front() == '-')) {
      // Optional sign
      Remaining = Remaining.drop_front(1);
    }

    // required exponent digits
    while (!Remaining.empty() && Remaining.front() >= '0' &&
           Remaining.front() <= '9') {
      IsMissingRequiredExponentDigits = false;
      Remaining = Remaining.drop_front(1);
    }
  }

  // We do not handle suffixes here as part of the literal. Instead, we let
  // suffixes be separate tokens and let the type checked interpret them
  Token T;
  T.Kind = tok::IntegerLiteral;

  if (StartsWithDigits) {
    // Undeniably a number
    if (HasDot || HasExponent) {
      T.Kind = tok::RealLiteral;
    }
  }
  else if (HasDot) {
    if (!HasFractionalDigits) {
      // just a '.'. Reset the buffer to its initial state
      Remaining = StartingBuff;
      return std::nullopt;
    }

    // Looks like a number after all
    T.Kind = tok::RealLiteral;
  }

  if (IsMissingRequiredFractionalDigits || IsMissingRequiredExponentDigits) {
    T.IsMalformed = true;
  }

  const llvm::StringRef Spelling = StartingBuff.drop_back(Remaining.size());
  T.Spelling = Spelling;
  T.SourceRange = { StartLoc, getCurrentSourceLoc() };

  return T;
}

} // namespace gstrands