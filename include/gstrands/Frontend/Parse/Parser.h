#pragma once


namespace gstrands {
class Lexer;

class Parser {
  public:
    explicit Parser(Lexer& Lex)
      : Lex(Lex) {}

    void parse();
  private:
    Lexer& Lex;
  };

} // namespace gstrands