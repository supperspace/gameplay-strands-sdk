#pragma once

namespace gstrands {

class Decl;

#define DECL(Name, ...) class Name##Decl;
#include "gstrands/AST/DeclNodes.inc"

} // namespace gstrands