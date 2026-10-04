#include "gstrands/Basic/ConsoleOutputDiagSink.h"

#include "llvm/Support/raw_ostream.h"

namespace gstrands {

void ConsoleOutputDiagSink::handleDiagnostic(const Diagnostic &Diag) {
  // obviously, a lot is left todo here
  llvm::outs() << Diag.Message << "\n";
}

} // namespace gstrands