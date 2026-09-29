include "llvm/TableGen/SearchableTable.td"


// Fixed spellings are complete lexemes. Empty means variable spelling
// or a synthetic token. Comments are retained as trivia.
class TokenKind<string spelling = ""> {
  string Spelling = spelling;
  TokenKind Kind = !cast<TokenKind>(NAME);
  bit IsKeyword = 0;
  bit IsTrivia = 0;
}

class Keyword<string spelling> : TokenKind<spelling> {
  let IsKeyword = 1;
}

class FundamentalType<string spelling> : Keyword<spelling>;
class TypeQualifier<string spelling> : Keyword<spelling>;
class Punctuator<string spelling> : TokenKind<spelling>;
class Operator<string spelling> : Punctuator<spelling>;

class Comment : TokenKind {
  let IsTrivia = 1;
}

// Synthetic tokens.
def Invalid   : TokenKind; // Previously not_really_a_token.
def EndOfFile : TokenKind;
def Unknown   : TokenKind;

// Comments and variable-spelling tokens.
def LineComment      : Comment;
def MultilineComment : Comment;

def Identifier      : TokenKind;
def IntegerLiteral  : TokenKind;
def RealLiteral     : TokenKind;
def DurationLiteral : TokenKind;

// Fundamental types.
def KwBool     : FundamentalType<"bool">;
def KwInt      : FundamentalType<"int">;
def KwFloat    : FundamentalType<"float">;
def KwDuration : FundamentalType<"duration">;

// Type qualifiers.
def KwConst   : TypeQualifier<"const">;
def KwDerived : TypeQualifier<"derived">;
def KwNative  : TypeQualifier<"native">;

// Other keywords.
def KwIf          : Keyword<"if">;
def KwElse        : Keyword<"else">;
def KwWhile       : Keyword<"while">;
def KwLet         : Keyword<"let">;
def KwExtern      : Keyword<"extern">;
def KwStruct      : Keyword<"struct">;
def KwTrait       : Keyword<"trait">;
def KwMutation    : Keyword<"mutation">;
def KwFunction    : Keyword<"function">;
def KwCollector   : Keyword<"collector">;
def KwDerive      : Keyword<"derive">;
def KwEvent       : Keyword<"event">;
def KwDelegate    : Keyword<"delegate">;
def KwExec        : Keyword<"exec">;
def KwNamespace   : Keyword<"namespace">;
def KwComponent   : Keyword<"component">;
def KwLink        : Keyword<"link">;
def KwImpl        : Keyword<"impl">;
def KwRollback    : Keyword<"rollback">;
def KwWith        : Keyword<"with">;
def KwUsing       : Keyword<"using">;
def KwHandle      : Keyword<"handle">;
def KwTransaction : Keyword<"transaction">;
def KwAuto        : Keyword<"auto">;
def KwExplicit    : Keyword<"explicit">;
def KwAsync       : Keyword<"async">;
def KwAwait       : Keyword<"await">;
def KwPreview     : Keyword<"preview">;
def KwReturn      : Keyword<"return">;
def KwBreak       : Keyword<"break">;
def KwContinue    : Keyword<"continue">;
def KwCommit      : Keyword<"commit">;
def KwEmit        : Keyword<"emit">;
def KwWrites      : Keyword<"writes">;
def KwControls    : Keyword<"controls">;
def KwProgram     : Keyword<"program">;
def KwWindow      : Keyword<"window">;
def KwPort        : Keyword<"port">;
def KwBind        : Keyword<"bind">;
def KwTo          : Keyword<"to">;
def KwOn          : Keyword<"on">;
def KwChannel     : Keyword<"channel">;

// Member access.
def Period : Punctuator<".">;
def Arrow  : Punctuator<"->">;

// Operators.
def PlusPlus   : Operator<"++">;
def MinusMinus : Operator<"--">;

def Exclaim : Operator<"!">;
def Tilde   : Operator<"~">;

def Plus  : Operator<"+">;
def Minus : Operator<"-">;

def Star    : Operator<"*">;
def Slash   : Operator<"/">;
def Percent : Operator<"%">;

def Less         : Operator<"<">;
def LessEqual    : Operator<"<=">;
def Greater      : Operator<">">;
def GreaterEqual : Operator<">=">;

def EqualEqual   : Operator<"==">;
def ExclaimEqual : Operator<"!=">;

def Amp   : Operator<"&">;
def Caret : Operator<"^">;
def Pipe  : Operator<"|">;

def AmpAmp   : Operator<"&&">;
def PipePipe : Operator<"||">;

// Assignment operators.
def Equal        : Operator<"=">;
def PlusEqual    : Operator<"+=">;
def MinusEqual   : Operator<"-=">;
def StarEqual    : Operator<"*=">;
def SlashEqual   : Operator<"/=">;
def PercentEqual : Operator<"%=">;
def AmpEqual     : Operator<"&=">;
def CaretEqual   : Operator<"^=">;
def PipeEqual    : Operator<"|=">;

// Delimiters and other punctuation.
def LSquare    : Punctuator<"[">;
def RSquare    : Punctuator<"]">;
def LParen     : Punctuator<"(">;
def RParen     : Punctuator<")">;
def LBrace     : Punctuator<"{">;
def RBrace     : Punctuator<"}">;
def ColonColon : Punctuator<"::">;
def Colon      : Punctuator<":">;
def Semicolon  : Punctuator<";">;
def Comma      : Punctuator<",">;
def At         : Punctuator<"@">;

def TokenKind: GenericEnum {
    let FilterClass = "TokenKind";
    let UnderlyingType = "uint16_t";
}

def TokenTable: GenericTable {
    let FilterClass = "TokenKind";
    let CppTypeName = "TokenInfo";

    let Fields = ["Kind", "Spelling", "IsKeyword", "IsTrivia"];

    string TypeOf_Kind = "TokenKind";

    let PrimaryKey = ["Kind"];
    let PrimaryKeyName = "lookupToken";
}

def KeywordTable: GenericTable {
    let FilterClass = "Keyword";
    let CppTypeName = "KeywordInfo";

    let Fields = ["Kind"];

    string TypeOf_Kind = "TokenKind";
}