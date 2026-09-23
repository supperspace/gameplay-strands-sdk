#include "gstrands/Frontend/CompilerInvocation.h"
#include "gstrands/Frontend/ProjectDefinition.h"

#include "llvm/Support/CommandLine.h"

using namespace llvm;

namespace {
cl::opt<std::string> BaseDir("base-dir", cl::desc("Base project dir"),
                             cl::Required);

cl::opt<std::string> OutputDir(cl::Positional, cl::desc("Output dir"),
                               cl::Required);

} // namespace

int main(const int Argc, const char *Argv[]) {

  cl::ParseCommandLineOptions(Argc, Argv);

  const gstrands::ProjectDefinition Project(BaseDir);

  auto ExpectedCompilerInvocation =
      gstrands::CompilerInvocation::createFromProjectDefinition(Project);

  if (ExpectedCompilerInvocation.takeError()) {
    return 1;
  }

  auto CompilerInvocation = std::move(ExpectedCompilerInvocation.get());

  return 0;
}