#pragma once
#include "gstrands/Basic/Diagnostics.h"

namespace llvm {
class raw_ostream;
}
namespace gstrands {
class SourceManager;

class StreamDiagConsumer: public DiagnosticsSink {
public:
  explicit StreamDiagConsumer(SourceManager& S, llvm::raw_ostream& Output)
    : SM(S)
    , OS(Output) {}

  ~StreamDiagConsumer() override = default;

  void handleDiagnostic(const Diagnostic &Diag) override;
private:
  SourceManager& SM;
  llvm::raw_ostream& OS;
};

}// namespace gstrands
