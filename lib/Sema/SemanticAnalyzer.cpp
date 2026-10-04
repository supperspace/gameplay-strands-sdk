#include "gstrands/Sema/SemanticAnalyzer.h"
#include "gstrands/Model/SemanticModel.h"

namespace gstrands {

SemanticAnalysisResult::SemanticAnalysisResult() = default;
SemanticAnalysisResult::SemanticAnalysisResult(SemanticAnalysisResult &&) noexcept = default;
SemanticAnalysisResult &SemanticAnalysisResult::operator=(SemanticAnalysisResult &&) noexcept = default;

SemanticAnalysisResult::~SemanticAnalysisResult() = default;

} // namespace gstrands