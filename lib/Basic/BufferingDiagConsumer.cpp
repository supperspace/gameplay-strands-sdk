#include "gstrands/Basic/BufferingDiagConsumer.h"

namespace gstrands {
BufferingDiagConsumer::~BufferingDiagConsumer() = default;

void BufferingDiagConsumer::handleDiagnostic(const Diagnostic &Diag) {
  Diags.push_back(Diag);
}

} // namespace gstrands