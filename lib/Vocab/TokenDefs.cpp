#include "gstrands/Vocab/TokenDefs.h"

namespace gstrands::tok {

#define GET_KeywordTable_IMPL
#define GET_TokenTable_IMPL
#include "gstrands/Vocab/TokenKinds.inc"

ArrayRef<KeywordInfo> getKeywords() {
  return KeywordTable;
}

} // namespace gstrands::tok