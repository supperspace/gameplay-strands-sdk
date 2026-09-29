#pragma once

namespace gstrands {

class Decl;

#define DECL(Name, ...) class Name##Decl;
#include "gstrands/Frontend/AST/DeclNodes.inc"

} // namespace gstrands