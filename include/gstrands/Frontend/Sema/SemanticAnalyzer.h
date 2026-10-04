#pragma once

#include "llvm/ADT/ArrayRef.h"

namespace gstrands {
class SemanticModel;
class ASTContext;

enum class SemaGoal {
  /// A Dependency only AST will be used only to satisfy dependencies coming from the analysis of other syntax trees
  DependencyOnly,
  /// A full
  FullAnalysis
};

/// Describes a single semantic analysis input. Wraps an AST and a goal for that respective AST. See @SemaGoal for
/// additional explanations
struct SemanticAnalysisInput {
  const ASTContext* AST = nullptr;
  SemaGoal Goal = SemaGoal::DependencyOnly;
};

struct SemanticAnalysisResult {
  std::unique_ptr<const SemanticModel> Model;

};

/// Encapsulates an entire semantic analysis pass, capable of synthesizing a model from a series of ASTs and an optional
/// model to be used as a baseline, case in which the compilation will be incremental
class SemanticAnalyzer {
public:
  /// Depending on SemanticAnalysisInput::Goal for each input, some inputs may not even be used for analysis
  SemanticAnalyzer(llvm::ArrayRef<const SemanticAnalysisInput> Inputs, const SemanticModel* IncrementalBaseline);

private:
};

} // namespace gstrands