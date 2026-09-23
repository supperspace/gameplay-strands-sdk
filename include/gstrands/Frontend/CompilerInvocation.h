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
class ProjectDefinition;

struct CompilationResult {

};

class CompilerInvocation {
public:
  static llvm::Expected<std::unique_ptr<CompilerInvocation>>
  createFromProjectDefinition(const ProjectDefinition &ProjectDef);

  CompilationResult compile();
private:
  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_reference_t<R>,
                                 llvm::StringRef>
  explicit CompilerInvocation(R &&InputSources) {
    if constexpr (std::ranges::sized_range<R>)
      SourceFiles.reserve(std::ranges::size(InputSources));

    for (auto &&Source : InputSources) {
      if constexpr (std::is_lvalue_reference_v<R>)
        SourceFiles.emplace_back(llvm::StringRef(Source));
      else
        SourceFiles.emplace_back(std::move(Source));
    }
  }

  llvm::SmallVector<llvm::SmallString<256>, 64> SourceFiles;

  llvm::BumpPtrAllocator Allocator;
  IdentifierTable Identifiers{Allocator};
  SourceManager SM;
};

} // namespace gstrands