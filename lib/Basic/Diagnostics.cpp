#include "gstrands/Basic/Diagnostics.h"

namespace gstrands {
namespace diag {
#define GET_DiagnosticsTable_IMPL
#include "gstrands/Basic/DiagnosticDefs.inc"

} // namespace diag

DiagnosticsSink::~DiagnosticsSink() = default;

DiagnosticBuilder::~DiagnosticBuilder() {
  Diagnostic Diag;
  Diag.Id = ID;
  Diag.Severity = Severity;
  Diag.Location = Location;

  Engine->emitDiagnostic(Diag);
}

DiagnosticBuilder DiagnosticsEngine::report(diag::DiagnosticId ID,
                                            SourceLocation Loc) {

  const diag::DiagnosticInfo *DiagInfo = diag::lookupDiagnosticInfo(ID);
  assert(DiagInfo != nullptr);

  return DiagnosticBuilder(*this, ID, DiagInfo->Severity, Loc);
}

void DiagnosticsEngine::emitDiagnostic(const Diagnostic &Diag) {
  if (Diag.Severity == diag::SEV_Fatal) {
    exit(-1);
  }

  if (Diag.Severity == diag::SEV_Error) {
    ++ErrorCount;
  }

  if (Diag.Severity == diag::SEV_Warning) {
    ++WarningCount;
  }

  Sink->handleDiagnostic(Diag);
}

} // namespace gstrands