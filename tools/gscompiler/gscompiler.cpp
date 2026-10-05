#include "gstrands/AST/Decl.h"
#include "gstrands/AST/RecursiveASTVisitor.h"
#include "gstrands/Basic/ConsoleOutputDiagSink.h"
#include "gstrands/Frontend/CompilerInvocation.h"
#include "gstrands/Model/SemanticModel.h"
#include "gstrands/Project/Project.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace llvm;

namespace {
cl::opt<std::string> BaseDir("base-dir", cl::desc("Base project dir"), cl::Required);

cl::opt<std::string> OutputDir(cl::Positional, cl::desc("Output dir"), cl::Required);

class DumpingASTVisitor : public gstrands::RecursiveASTVisitor<DumpingASTVisitor> {
public:
  using Base = RecursiveASTVisitor<DumpingASTVisitor>;

  bool visitNamespaceDecl(const gstrands::NamespaceDecl *D) const {
    dump() << "namespace " << D->getNameSpelling() << "\n";
    return true;
  }

  bool visitComponentDecl(const gstrands::ComponentDecl *D) const {
    dump() << "component " << D->getNameSpelling() << "\n";
    return true;
  }

  bool visitTraitDecl(const gstrands::TraitDecl *D) const {
    dump() << "trait " << D->getNameSpelling() << "\n";
    return true;
  }

  bool visitImplDecl(const gstrands::ImplDecl *D) const {
    dump() << "impl\n";
    return true;
  }

  bool visitChannelDecl(const gstrands::ChannelDecl *D) const {
    dump() << "channel " << D->getNameSpelling() << "\n";
    return true;
  }

  bool traverseDeclContext(const gstrands::DeclContext *D) {
    ++Indents;
    const auto R = Base::traverseDeclContext(D);
    --Indents;
    return R;
  }

  raw_ostream &dump() const { return outs().indent(Indents * 2); }

  size_t Indents = 0;
};

} // namespace

int main(const int Argc, const char *Argv[]) {
  cl::ParseCommandLineOptions(Argc, Argv);

  const gstrands::Project Project(BaseDir, llvm::vfs::getRealFileSystem());

  auto Inventory = Project.createSourceInventory();
  if (auto EC = Inventory.takeError(); EC) {
    return 1;
  }

  BumpPtrAllocator IdentAlloc;

  gstrands::SourceManager SrcMgr(llvm::vfs::getRealFileSystem());
  gstrands::ConsoleOutputDiagSink ConsoleDiagConsumer(SrcMgr);
  gstrands::IdentifierTable IdentTab(IdentAlloc);
  gstrands::SemanticModel DummyModel;

  auto Invocation =
      gstrands::CompilerInvocation(std::move(Inventory.get()), DummyModel, ConsoleDiagConsumer, IdentTab, SrcMgr);
  const gstrands::CompilationResult Result = Invocation.compile();

  DumpingASTVisitor Visitor;
  for (const auto &AST : Result.ASTs) {
    Visitor.traverseAST(*AST);
  }

  return 0;
}