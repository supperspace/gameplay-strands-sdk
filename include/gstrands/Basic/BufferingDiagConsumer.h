#pragma once
#include "gstrands/Basic/Diagnostics.h"

namespace gstrands {

class BufferingDiagConsumer: public DiagnosticsSink {
public:
  ~BufferingDiagConsumer() override;

  void handleDiagnostic(const Diagnostic &Diag) override;

  llvm::ArrayRef<Diagnostic> getDiags() const {
    return Diags;
  }

  
private:
  std::vector<Diagnostic> Diags;
};
} // namespace gstrands