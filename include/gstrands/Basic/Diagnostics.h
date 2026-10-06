#pragma once

#include "gstrands/Basic/SourceLocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringTable.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdint>

namespace gstrands {
class SourceManager;

namespace diag {
using namespace llvm;

#define GET_DiagnosticId_DECL
#define GET_DiagnosticClass_DECL
#define GET_DiagnosticSeverity_DECL
#include "gstrands/Basic/DiagnosticDefs.inc"

struct DiagnosticInfo {
  DiagnosticId ID;
  StringTable::Offset Offset;
  DiagnosticClass Class;
  DiagnosticSeverity Severity;
};

#define GET_DiagnosticsTable_DECL
#include "gstrands/Basic/DiagnosticDefs.inc"

} // namespace diag

struct Diagnostic {
  diag::DiagnosticId Id;
  diag::DiagnosticSeverity Severity;
  SourceLocation Location;
  std::string Message;
};

class DiagnosticsSink {
public:
  virtual ~DiagnosticsSink() = 0;

  virtual void handleDiagnostic(const Diagnostic &Diag) = 0;
};

class DiagnosticsEngine;
class DiagnosticBuilder {
  friend class DiagnosticsEngine;

public:
  ~DiagnosticBuilder();

  DiagnosticBuilder(const DiagnosticBuilder&) = delete;
  DiagnosticBuilder& operator=(const DiagnosticBuilder&) = delete;

  template <typename T>
  friend llvm::raw_ostream &operator<<(DiagnosticBuilder &&DB, T&& O) {
    return DB.MessageStream << std::forward<T>(O);
  }

private:
  explicit DiagnosticBuilder(DiagnosticsEngine &E, const diag::DiagnosticId I,
                             const diag::DiagnosticSeverity Sev,
                             SourceLocation Loc)
      : Engine(&E), ID(I), Severity(Sev), Location(Loc) {}

  DiagnosticsEngine *Engine = nullptr;
  diag::DiagnosticId ID;
  diag::DiagnosticSeverity Severity;
  SourceLocation Location;

  llvm::SmallString<256> MessageBuff;
  llvm::raw_svector_ostream MessageStream{MessageBuff};
};

class DiagnosticsEngine {
  friend class DiagnosticBuilder;

public:
  explicit DiagnosticsEngine(DiagnosticsSink &S) : Sink(&S) {}

  DiagnosticBuilder report(diag::DiagnosticId ID, SourceLocation Loc);

private:
  void emitDiagnostic(const Diagnostic &Diag);

  DiagnosticsSink *Sink = nullptr;

  size_t ErrorCount = 0;
  size_t WarningCount = 0;
};

} // namespace gstrands