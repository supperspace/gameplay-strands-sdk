#include "gstrands/Frontend/Lex/Token.h"
#include "gstrands/Frontend/Lex/TokenDefs.h"

namespace gstrands::tok {

#define GET_KeywordTable_IMPL
#define GET_TokenTable_IMPL
#include "TokenKinds.inc"

ArrayRef<KeywordInfo> getKeywords() {
  return KeywordTable;
}

} // namespace gstrands::tok