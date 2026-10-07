#include "gstrands/Frontend/CompilerInvocation.h"

#include "gstrands/AST/ASTContext.h"
#include "gstrands/Basic/Diagnostics.h"
#include "gstrands/Basic/StreamDiagConsumer.h"
#include "gstrands/Lex/Lexer.h"
#include "gstrands/Model/SemanticModel.h"
#include "gstrands/Parse/Parser.h"
#include "gstrands/Project/Project.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

#include <set>

namespace gstrands {

CompilationResult::CompilationResult() = default;
CompilationResult::CompilationResult(CompilationResult &&) noexcept = default;
CompilationResult &CompilationResult::operator=(CompilationResult &&) noexcept = default;

CompilationResult::~CompilationResult() = default;

CompilerInvocation::CompilerInvocation(SourceInventory Inventory, SemanticModel &Baseline,
                                       DiagnosticsSink &DiagConsumer, IdentifierTable &Idents, SourceManager &SrcMgr)
    : InputInventory(std::move(Inventory)), BaselineModel(Baseline), Identifiers(Idents), SM(SrcMgr),
      DiagEngine(std::make_unique<DiagnosticsEngine>(DiagConsumer)) {}

CompilerInvocation::~CompilerInvocation() = default;

CompilationResult CompilerInvocation::compile() const {
  CompilationResult Result;

  for (const auto& [SourcePath, Signature] : InputInventory.getSources()) {
    const auto OldSignature = BaselineModel.getSourceSignature(std::string_view(SourcePath));
    bool ShouldParse = false;

    if (OldSignature) {
      // This file did exist before.
      if (OldSignature == Signature) {
        // The file is up to date
        ++Result.Stats.CountUpToDateFiles;
      }
      else {
        // File is changed, and needs to be parsed again (ofc, this is a naive check only. the up to date check needs improvements)
        ShouldParse = true;
      }
    }
    else {
      // This looks like a new file
      ++Result.Stats.CountNewFiles;
      ShouldParse = true;
    }

    if (ShouldParse) {
      auto SourceBuffer = SM.getOrLoadBuffer(SourcePath);
      if (SourceBuffer.takeError()) {
        return {}; // This would be a fatal error
      }

      Result.ASTs.emplace_back(std::make_unique<ASTContext>(SourceBuffer.get().BaseLocation));
      Lexer Lex{SourceBuffer.get(), Identifiers};
      Parser Parse{Lex, *DiagEngine, *Result.ASTs.back(), Identifiers};

      Parse.parse();
      std::array<uint8_t, 8> LexBlake3;
      Lex.takeFinalBlake3(LexBlake3);

      llvm::errs() << "Finished parsing '" << SourcePath << "'. Blake3: '" << llvm::toHex(LexBlake3) << "'\n";
    }
  }

  return Result;
}

} // namespace gstrands