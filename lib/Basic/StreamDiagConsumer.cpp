#include "gstrands/Basic/StreamDiagConsumer.h"

#include "llvm/Support/raw_ostream.h"

namespace gstrands {

void StreamDiagConsumer::handleDiagnostic(const Diagnostic &Diag) {
  // obviously, a lot is left todo here
  OS << Diag.Message << "\n";
}

} // namespace gstrands