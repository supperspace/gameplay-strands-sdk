#include "gstrands/Parse/ParserPolicy.h"

#include "llvm/ADT/ArrayRef.h"

#include <algorithm>

namespace gstrands {

using llvm::ArrayRef;

// Generated initializers use unqualified names such as KwTrait.
using namespace gstrands::tok;

#define GET_DeclSyntaxTable_IMPL
#define GET_DeclNestingTable_IMPL
#include "gstrands/Parse/ParserPolicy.inc"

} // namespace gstrands