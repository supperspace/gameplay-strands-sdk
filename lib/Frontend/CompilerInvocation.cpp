#include "gstrands/Frontend/CompilerInvocation.h"

#include "gstrands/Lex/Lexer.h"
#include "gstrands/Parse/Parser.h"
#include "gstrands/Project/Project.h"
#include "gstrands/AST/ASTContext.h"
#include "gstrands/Basic/ConsoleOutputDiagSink.h"
#include "gstrands/Basic/Diagnostics.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

#include <set>

namespace gstrands {

CompilationResult::CompilationResult() = default;
CompilationResult::CompilationResult(CompilationResult&&) noexcept = default;
CompilationResult &CompilationResult::operator=(CompilationResult&&) noexcept = default;

CompilationResult::~CompilationResult() = default;

CompilerInvocation::CompilerInvocation(SourceInventory Inventory, DiagnosticsSink &DiagConsumer,
                                       IdentifierTable &Idents, SourceManager &SrcMgr)
    : InputInventory(std::move(Inventory)), Identifiers(Idents), SM(SrcMgr),
      DiagEngine(std::make_unique<DiagnosticsEngine>(DiagConsumer)) {}

CompilerInvocation::~CompilerInvocation() = default;

CompilationResult CompilerInvocation::compile() const {
  CompilationResult Result;

  for (const auto& Source : InputInventory.getSources()) {

    auto SourceBuffer = SM.getOrLoadBuffer(Source.NormPath);
    if (SourceBuffer.takeError()) {
      return {}; // This would be a fatal error
    }

    Result.ASTs.emplace_back(std::make_unique<ASTContext>(SourceBuffer.get().BaseLocation));
    Lexer Lex{SourceBuffer.get(), Identifiers};
    Parser Parse{Lex, *DiagEngine, *Result.ASTs.back()};

    Parse.parse();
  }

  return Result;
}


} // namespace gstrands