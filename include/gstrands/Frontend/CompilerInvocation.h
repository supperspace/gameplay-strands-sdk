#pragma once

#include "gstrands/Basic/IdentifierTable.h"
#include "gstrands/Basic/SourceManager.h"
#include "gstrands/Project/SourceInventory.h"

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
class Project;

struct CompilationResult {
  CompilationResult();
  CompilationResult(const CompilationResult&) = delete;
  CompilationResult(CompilationResult&&) noexcept;

  CompilationResult &operator=(const CompilationResult&) = delete;
  CompilationResult &operator=(CompilationResult&&) noexcept;

  ~CompilationResult();

  std::vector<std::unique_ptr<ASTContext>> ASTs;
};

class CompilerInvocation {
public:
  CompilerInvocation() = delete;
  CompilerInvocation(SourceInventory Inventory, DiagnosticsSink& DiagConsumer, IdentifierTable& Idents, SourceManager& SrcMgr);

  ~CompilerInvocation();

  [[nodiscard]] CompilationResult compile() const;

private:
  SourceInventory InputInventory;

  llvm::BumpPtrAllocator Allocator;

  IdentifierTable& Identifiers;
  SourceManager& SM;
  std::unique_ptr<DiagnosticsEngine> DiagEngine;
};

} // namespace gstrands