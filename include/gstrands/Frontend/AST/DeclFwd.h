#pragma once

namespace gstrands {

#define ABSTRACT_DECL(...)
#define DECL(Name, ...) class Name##Decl;
#include "gstrands/Frontend/AST/DeclNodes.inc"

} // namespace gstrands