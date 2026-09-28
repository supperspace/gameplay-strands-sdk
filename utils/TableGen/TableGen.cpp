#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/TableGen/Main.h"

int main(int argc, char **argv) {
  llvm::InitLLVM Init(argc, argv);

  llvm::cl::ParseCommandLineOptions(argc, argv, "Gameplay Strands TableGen\n");

  llvm::MultiFileTableGenMainFn Fallback = nullptr;
  return llvm::TableGenMain(argv[0], Fallback);
}