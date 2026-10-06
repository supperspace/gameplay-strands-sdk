#pragma once

#include "llvm/ADT/StringTable.h"
#include "llvm/ADT/ArrayRef.h"

#include <cstdint>

namespace gstrands::tok {
using namespace llvm;

enum TokenKind: uint16_t;

struct KeywordInfo {
  TokenKind Kind;
};

struct TokenInfo {
  TokenKind Kind;
  StringTable::Offset Offset;
  bool IsKeyword;
  bool IsTrivia;
};

ArrayRef<KeywordInfo> getKeywords();

#define GET_TokenKind_DECL
#define GET_TokenTable_DECL
#define GET_KeywordTable_DECL
#define GET_DeclKeywordTable_DECL

#include "gstrands/Vocab/TokenKinds.inc"

} // namespace gstrands::tok