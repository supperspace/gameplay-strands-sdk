#include "gstrands/Frontend/AST/ASTContext.h"

#include "gstrands/Frontend/AST/Decl.h"

namespace gstrands {

ASTContext::ASTContext(const SourceLocation RootL) {
  Root = &emplace<RootDecl>(RootL);
}

} // namespace gstrands