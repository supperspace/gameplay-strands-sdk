#include "gstrands/Frontend/Basic/ConsoleOutputDiagSink.h"

namespace gstrands {

void ConsoleOutputDiagSink::handleDiagnostic(const Diagnostic &Diag) {
  // obviously, a lot is left todo here
  llvm::outs() << Diag.Message;
}

} // namespace gstrands