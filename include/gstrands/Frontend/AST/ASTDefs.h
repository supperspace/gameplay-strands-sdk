#pragma once
#include <cstdint>

namespace gstrands {

enum class DeclKind: uint8_t {
  Unknown,
#define AST_NODE(Name, ...) Name,
#include "gstrands/Frontend/AST/DeclNodes.inc"

};


} // namespace gstrands