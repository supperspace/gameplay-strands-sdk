#include "gstrands/AST/DeclBase.h"
#include "gstrands/AST/Decl.h"

namespace gstrands {
DeclContext *Decl::asDeclContext() {
  switch (getKind()) {
#define ABSTRACT_DECL(...)
#define DECL(...)
#define DECL_CONTEXT(Name) case Name: return static_cast<Name##Decl*>(this);
#include "gstrands/AST/DeclNodes.inc"
  default: return nullptr;
  }

}

} // namespace gstrands