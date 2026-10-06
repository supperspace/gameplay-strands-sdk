#include "gstrands/Basic/BufferingDiagConsumer.h"
#include "gstrands/Frontend/CompilerInvocation.h"
#include "gstrands/Frontend/Workspace.h"
#include "gstrands/Model/SemanticModel.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/LSP/Transport.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/TableGen/Error.h"

using namespace llvm;

namespace {
class StrandsLSPHandler {
public:
  explicit StrandsLSPHandler(gstrands::Workspace &WS) : W(WS) {}

  void handleInitialize(const lsp::InitializeParams &InitParams, lsp::Callback<json::Object> CB) {
    errs() << "LSP Client initialization\n";
    if (InitParams.clientInfo) {
      errs() << "Client Info:\n";

      auto &ClientInfo = *InitParams.clientInfo;
      errs().indent(2) << "Name: " << ClientInfo.name << "\n";
      errs().indent(2) << "Version: " << (ClientInfo.version ? StringRef(*ClientInfo.version) : StringRef("")) << "\n";
    } else {
      errs() << "Client info missing\n";
    }

    if (InitParams.rootUri) {
      errs() << "Root Uri: " << *InitParams.rootUri << "\n";
    } else {
      errs() << "No root uri provided\n";
    }

    if (InitParams.rootPath) {
      errs() << "Root Path: " << *InitParams.rootPath << "\n";
    } else {
      errs() << "No root path provided\n";
    }

    if (InitParams.trace) {
      StringRef StrTraceLevel = "off";
      switch (*InitParams.trace) {
      case lsp::TraceLevel::Verbose:
        StrTraceLevel = "verbose";
        break;
      case lsp::TraceLevel::Messages:
        StrTraceLevel = "messages";
        break;
      default:
        break;
      }

      errs() << "Setting trace level to: " << StrTraceLevel << "\n";
      switch (*InitParams.trace) {
      case lsp::TraceLevel::Verbose:
        lsp::Logger::setLogLevel(lsp::Logger::Level::Debug);
        break;
      case lsp::TraceLevel::Messages:
        lsp::Logger::setLogLevel(lsp::Logger::Level::Info);
        break;
      default:
        lsp::Logger::setLogLevel(lsp::Logger::Level::Error);
        break;
      }
    } else {
      errs() << "No trace level specified, defaulting to 'verbose'\n";
      lsp::Logger::setLogLevel(lsp::Logger::Level::Debug);
    }

    CB(json::Object{{.K = "capabilities",
                     .V = json::Object{{.K = "textDocumentSync",
                                        .V = json::Object{{.K = "openClose", .V = true}, {.K = "change", .V = 1}}}}}});
  }

  void handleShutdown(const std::nullptr_t &, lsp::Callback<json::Value> CB) {
    lsp::Logger::debug("Shutdown requested");

    CB(nullptr);
  }

  void handleDidOpen(const lsp::DidOpenTextDocumentParams &DidOpenParams) {
    processNewSourceSnapshot(W.fileOpened(DidOpenParams.textDocument.uri.file(), DidOpenParams.textDocument.text,
                                          DidOpenParams.textDocument.version));
  }

  void handleDidChange(const lsp::DidChangeTextDocumentParams &DidChangeParams) {
    std::string CurrentContents = W.getDraftCopy(DidChangeParams.textDocument.uri.file());

    if (const auto ApplyResult =
            lsp::TextDocumentContentChangeEvent::applyTo(DidChangeParams.contentChanges, CurrentContents);
        ApplyResult.succeeded()) {

      processNewSourceSnapshot(W.fileChanged(DidChangeParams.textDocument.uri.file(), CurrentContents,
                                             DidChangeParams.textDocument.version));
    }
  }

  void handleDidSave(const lsp::DidSaveTextDocumentParams &DidSaveParams) {
    processNewSourceSnapshot(W.fileSaved(DidSaveParams.textDocument.uri.file()));
  }

  void handleDidClose(const lsp::DidCloseTextDocumentParams &DidCloseParams) {
    processNewSourceSnapshot(W.fileClosed(DidCloseParams.textDocument.uri.file()));
  }

  lsp::OutgoingNotification<lsp::PublishDiagnosticsParams> PublishDiagnosticsCB;

private:
  void processNewSourceSnapshot(gstrands::SourceSnapshot Snapshot) {
    BumpPtrAllocator Alloc;
    gstrands::BufferingDiagConsumer DiagConsumer;
    gstrands::IdentifierTable IdentTab(Alloc);
    gstrands::SemanticModel DummyModel;
    auto &SrcMgr = Snapshot.getSourceManager();

    auto FS = Snapshot.getFilesystem();
    for (const auto &P : Snapshot.getProjects()) {
      auto Inventory = P.createSourceInventory(*FS);
      if (!Inventory.takeError()) {
        auto Invocation =
            gstrands::CompilerInvocation(std::move(Inventory.get()), DummyModel, DiagConsumer, IdentTab, SrcMgr);

        const gstrands::CompilationResult Result = Invocation.compile();
      }
    }

    if (PublishDiagnosticsCB) {
      for (const auto& [Path, Metadata]: Snapshot.getSourceDrafts()) {
        if (auto Uri = lsp::URIForFile::fromFile(Path); !Uri.takeError()) {
          lsp::PublishDiagnosticsParams PublishParams(Uri.get(), Metadata.Version);

          const auto File = SrcMgr.getFileId(Path);
          for (const auto& Diag: DiagConsumer.getDiags()) {
            const auto DiagFile = SrcMgr.getFileForLocation(Diag.Location);
            if (DiagFile == File) {
              auto ExpandedLoc = SrcMgr.expandSourceLocation(Diag.Location);

              lsp::Diagnostic LspDiag;
              // much todo
              LspDiag.message = Diag.Message;
              LspDiag.severity = lsp::DiagnosticSeverity::Error; // todo
              // Note an expanded source location currently has byte offsets while our source might use a different encoding. So this is known incorrect
              LspDiag.range = lsp::Range(lsp::Position(ExpandedLoc.Row, ExpandedLoc.Column));
              PublishParams.diagnostics.push_back(LspDiag);
            }
          }

          PublishDiagnosticsCB(PublishParams);
        }
        else {
          errs() << Uri.takeError() << "\n";
        }
      }
    }

    // Now we should be in a position to send back diagnostics, but we need more QoL, such as filtering out diags from
    // files that are not of interest (ie not open) and the ability to group diagnostics per file.
  }

  gstrands::Workspace &W;
};

} // namespace

int main(int Argc, const char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv);

  errs() << "Running in LSP mode\n";

  // Change stdin to binary mode so line endings stay predictable
  sys::ChangeStdinToBinary();

  lsp::JSONTransport LSPTransport(stdin, llvm::outs());
  lsp::MessageHandler Handler(LSPTransport);
  gstrands::Workspace LSPWorkspace;

  StrandsLSPHandler LSPHandler(LSPWorkspace);

  Handler.method("initialize", &LSPHandler, &StrandsLSPHandler::handleInitialize);
  Handler.method("shutdown", &LSPHandler, &StrandsLSPHandler::handleShutdown);

  Handler.notification("textDocument/didOpen", &LSPHandler, &StrandsLSPHandler::handleDidOpen);
  Handler.notification("textDocument/didChange", &LSPHandler, &StrandsLSPHandler::handleDidChange);
  Handler.notification("textDocument/didSave", &LSPHandler, &StrandsLSPHandler::handleDidSave);
  Handler.notification("textDocument/didClose", &LSPHandler, &StrandsLSPHandler::handleDidClose);

  LSPHandler.PublishDiagnosticsCB =
      Handler.outgoingNotification<lsp::PublishDiagnosticsParams>("textDocument/publishDiagnostics");

  if (auto Err = LSPTransport.run(Handler); Err) {
    errs() << "LSP transport failed: " << toString(std::move(Err)) << "\n";
    return 1;
  }

  errs() << "LSP transport stopped normally\n";

  return 0;
}