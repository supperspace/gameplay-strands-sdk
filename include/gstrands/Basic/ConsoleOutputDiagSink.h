#pragma once
#include "gstrands/Basic/Diagnostics.h"

namespace gstrands {
class SourceManager;

class ConsoleOutputDiagSink: public DiagnosticsSink {
public:
  explicit ConsoleOutputDiagSink(SourceManager& S)
    : SM(S) {}

  ~ConsoleOutputDiagSink() override = default;

  void handleDiagnostic(const Diagnostic &Diag) override;
private:
  SourceManager& SM;
};

}// namespace gstrands
