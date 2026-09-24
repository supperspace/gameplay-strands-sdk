#include "gstrands/Frontend/Basic/IdentifierTable.h"
#include "gstrands/Frontend/Lex/TokenDefs.h"

namespace gstrands {

void IdentifierTable::addKeywords() {

  for (auto KwInfo: tok::getKeywords()) {
    const auto* TokenInfo = tok::lookupToken(KwInfo.Kind);
    assert(TokenInfo != nullptr);

    auto Str = tok::getTokenInfoStr(TokenInfo->Offset);

    auto Ident = get(Str);
    Ident.Info->K = KwInfo.Kind;
  }
}

} // namespace gstrands