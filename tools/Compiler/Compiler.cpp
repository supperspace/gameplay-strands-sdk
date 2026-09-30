#include "gstrands/Frontend/AST/RecursiveASTVisitor.h"
#include "gstrands/Frontend/CompilerInvocation.h"
#include "gstrands/Frontend/ProjectDefinition.h"

#include "llvm/Support/CommandLine.h"

using namespace llvm;

namespace {
cl::opt<std::string> BaseDir("base-dir", cl::desc("Base project dir"), cl::Required);

cl::opt<std::string> OutputDir(cl::Positional, cl::desc("Output dir"), cl::Required);

class DumpingASTVisitor : public gstrands::RecursiveASTVisitor<DumpingASTVisitor> {
public:
  using Base = RecursiveASTVisitor<DumpingASTVisitor>;

  bool visitNamespaceDecl(const gstrands::NamespaceDecl *D) const {
    dump() << "namespace\n";
    return true;
  }

  bool visitComponentDecl(const gstrands::ComponentDecl *D) const {
    dump() << "component\n";
    return true;
  }

  bool visitTraitDecl(const gstrands::TraitDecl *D) const {
    dump() << "trait\n";
    return true;
  }

  bool visitImplDecl(const gstrands::ImplDecl *D) const {
    dump() << "impl\n";
    return true;
  }

  bool visitChannelDecl(const gstrands::ChannelDecl *D) const {
    dump() << "channel\n";
    return true;
  }

  bool traverseDeclContext(const gstrands::DeclContext *D) {
    ++Indents;
    const auto R = Base::traverseDeclContext(D);
    --Indents;
    return R;
  }

  raw_ostream &dump() const {
    return outs().indent(Indents * 2);
  }

  size_t Indents = 0;
};

} // namespace

int main(const int Argc, const char *Argv[]) {

  cl::ParseCommandLineOptions(Argc, Argv);

  const gstrands::ProjectDefinition Project(BaseDir);

  auto ExpectedCompilerInvocation = gstrands::CompilerInvocation::createFromProjectDefinition(Project);

  if (ExpectedCompilerInvocation.takeError()) {
    return 1;
  }

  const gstrands::CompilationResult Result = ExpectedCompilerInvocation.get()->compile();

  DumpingASTVisitor Visitor;
  for (const auto &AST : Result.ASTs) {
    Visitor.traverseAST(*AST);
  }
  return 0;
}