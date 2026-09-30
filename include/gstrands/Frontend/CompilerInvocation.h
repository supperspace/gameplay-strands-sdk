#pragma once

#include "Basic/IdentifierTable.h"
#include "Basic/SourceManager.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/Support/Allocator.h"

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

namespace gstrands {
class ASTContext;
class DiagnosticsSink;
class DiagnosticsEngine;
class ProjectDefinition;

struct CompilationResult {
  std::vector<std::unique_ptr<ASTContext>> ASTs;
};

class CompilerInvocation {
public:
  ~CompilerInvocation();

  static llvm::Expected<std::unique_ptr<CompilerInvocation>>
  createFromProjectDefinition(const ProjectDefinition &ProjectDef);

  [[nodiscard]] CompilationResult compile();
private:
  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_reference_t<R>,
                                 llvm::StringRef>
  explicit CompilerInvocation(R &&InputSources)
      : CompilerInvocation() {
    if constexpr (std::ranges::sized_range<R>)
      SourceFiles.reserve(std::ranges::size(InputSources));

    for (auto &&Source : InputSources) {
      if constexpr (std::is_lvalue_reference_v<R>)
        SourceFiles.emplace_back(llvm::StringRef(Source));
      else
        SourceFiles.emplace_back(std::move(Source));
    }
  }

  CompilerInvocation();

  llvm::SmallVector<llvm::SmallString<256>, 64> SourceFiles;

  llvm::BumpPtrAllocator Allocator;

  std::unique_ptr<IdentifierTable> Identifiers;
  std::unique_ptr<SourceManager> SM;
  std::unique_ptr<DiagnosticsSink> DiagSink;
  std::unique_ptr<DiagnosticsEngine> DiagEngine;
};

} // namespace gstrands