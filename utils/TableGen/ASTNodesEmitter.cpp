// ASTNodesEmitter.cpp
#include "llvm/Support/raw_ostream.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"

using namespace llvm;

static void emitASTNodes(const RecordKeeper &Records, raw_ostream &OS) {
  emitSourceFileHeader("GameplayStrands AST nodes", OS, Records);

  OS << "#ifndef AST_NODE\n";
  OS << "#define AST_NODE(...)\n";
  OS << "#endif\n";

  OS << "\n\n";

  auto Nodes =
    to_vector(Records.getAllDerivedDefinitions("ASTNode"));

  sort(Nodes, llvm::LessRecordByID{});

  for (const Record *Node : Nodes) {
    const Record *Base = Node->getValueAsOptionalDef("Base");

    OS << "AST_NODE(" << Node->getName() << ", ";
    if (Base)
      OS << Base->getName();
    else
      OS << "None";
    OS << ", " << (Node->getValueAsBit("Abstract") ? 1 : 0) << ")\n";
  }

  OS << "\n\n";
  OS << "#undef AST_NODE" << "\n";
}

namespace {

TableGen::Emitter::Opt
    ASTNodes("gen-ast-nodes", emitASTNodes,
             "Generate GameplayStrands AST node information");

} // namespace