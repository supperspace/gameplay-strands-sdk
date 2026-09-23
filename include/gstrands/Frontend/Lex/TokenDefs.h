#pragma once
#include <cstdint>

namespace gstrands {

enum class TokenKind: uint16_t
{
  InvalidToken = 0,
#include "TokenKinds.inc"
};

} // namespace gstrands