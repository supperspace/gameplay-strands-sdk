#include "gstrands/Frontend/CompilerInvocation.h"

#include "gstrands/Frontend/Lex/Lexer.h"
#include "gstrands/Frontend/Parse/Parser.h"
#include "gstrands/Frontend/ProjectDefinition.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

#include <set>

namespace gstrands {

llvm::Expected<std::unique_ptr<CompilerInvocation>>
CompilerInvocation::createFromProjectDefinition(
    const ProjectDefinition &ProjectDef) {

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
  for (llvm::StringRef Source : SourceFiles) {

    auto SourceBuffer = SM.getOrLoadBuffer(Source);
    if (SourceBuffer.takeError()) {
      return {}; // This would be a fatal error
    }

    Lexer Lex{SourceBuffer.get(), Identifiers};
    Parser Parse{Lex};

    Parse.parse();
  }

  return {};
}

} // namespace gstrands