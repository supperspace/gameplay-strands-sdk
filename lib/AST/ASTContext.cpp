#include "gstrands/AST/ASTContext.h"

#include "gstrands/AST/Decl.h"

namespace gstrands {

ASTContext::ASTContext(const SourceLocation RootL) {
  Root = &emplace<RootDecl>(RootL);
}

} // namespace gstrands