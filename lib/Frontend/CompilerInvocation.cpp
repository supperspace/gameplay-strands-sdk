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

CompilerInvocation::CompilerInvocation()
    : Identifiers(std::make_unique<IdentifierTable>(Allocator)),
      SM(std::make_unique<SourceManager>(llvm::vfs::getRealFileSystem())),
      DiagSink(std::make_unique<ConsoleOutputDiagSink>(*SM)),
      DiagEngine(std::make_unique<DiagnosticsEngine>(*DiagSink)) {}

CompilationResult::CompilationResult() = default;
CompilationResult::CompilationResult(CompilationResult&&) noexcept = default;
CompilationResult &CompilationResult::operator=(CompilationResult&&) noexcept = default;

CompilationResult::~CompilationResult() = default;

CompilerInvocation::~CompilerInvocation() = default;

llvm::Expected<std::unique_ptr<CompilerInvocation>>
CompilerInvocation::createFromProjectDefinition(
    const Project &ProjectDef) {

  std::error_code EC;
  const llvm::sys::fs::recursive_directory_iterator EndIterator = {};
  llvm::sys::fs::recursive_directory_iterator Iterator(
      ProjectDef.getBaseDirectory(), EC, false);

  llvm::SmallString<256> CurrentPath;
  if (EC = llvm::sys::fs::current_path(CurrentPath); EC)
    return llvm::errorCodeToError(EC);

  llvm::SmallString<256> AbsoluteBase{ProjectDef.getBaseDirectory()};
  llvm::sys::path::make_absolute(CurrentPath, AbsoluteBase);

  if (EC)
    return llvm::errorCodeToError(EC);

  std::set<llvm::SmallString<256>> InputSet;

  while (Iterator != EndIterator) {
    if (Iterator->type() == llvm::sys::fs::file_type::regular_file) {
      if (llvm::sys::path::extension(Iterator->path()) == ".gss") {

        // Add it to the input set

        // Start by normalizing the path
        llvm::SmallString<256> Absolute{Iterator->path()};

        llvm::sys::path::make_absolute(AbsoluteBase, Absolute);

        llvm::SmallString<256> Resolved;
        if (EC = llvm::sys::fs::real_path(Absolute, Resolved); EC)
          return llvm::errorCodeToError(EC);

        llvm::sys::path::make_preferred(Resolved);
        InputSet.emplace(std::move(Resolved));
      }
    }

    Iterator = Iterator.increment(EC);
    if (EC)
      return llvm::errorCodeToError(EC);
  }

  return std::unique_ptr<CompilerInvocation>{new CompilerInvocation(InputSet)};
}

CompilationResult CompilerInvocation::compile() {
  CompilationResult Result;

  for (llvm::StringRef Source : SourceFiles) {

    auto SourceBuffer = SM->getOrLoadBuffer(Source);
    if (SourceBuffer.takeError()) {
      return {}; // This would be a fatal error
    }

    Result.ASTs.emplace_back(std::make_unique<ASTContext>(SourceBuffer.get().BaseLocation));
    Lexer Lex{SourceBuffer.get(), *Identifiers};
    Parser Parse{Lex, *DiagEngine, *Result.ASTs.back()};

    Parse.parse();
  }

  return Result;
}


} // namespace gstrands