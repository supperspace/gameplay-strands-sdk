#include "gstrands/Frontend/CompilerInvocation.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/LSP/Transport.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace llvm;

namespace {
class StrandsLSPHandler {
public:
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

  void handleDidOpen(const lsp::DidOpenTextDocumentParams& DidOpenParams) {}
  void handleDidChange(const lsp::DidChangeTextDocumentParams& DidChangeParams) {}

protected:

};

} // namespace

int main(int Argc, const char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv);

  errs() << "Running in LSP mode\n";

  // Change stdin to binary mode so line endings stay predictable
  sys::ChangeStdinToBinary();

  lsp::JSONTransport LSPTransport(stdin, llvm::outs());
  lsp::MessageHandler Handler(LSPTransport);
  StrandsLSPHandler LSPHandler;

  Handler.method("initialize", &LSPHandler, &StrandsLSPHandler::handleInitialize);
  Handler.method("shutdown", &LSPHandler, &StrandsLSPHandler::handleShutdown);

  Handler.notification("textDocument/didOpen", &LSPHandler, &StrandsLSPHandler::handleDidOpen);
  Handler.notification("textDocument/didChange", &LSPHandler, &StrandsLSPHandler::handleDidChange);


  if (auto Err = LSPTransport.run(Handler); Err) {
    errs() << "LSP transport failed: " << toString(std::move(Err)) << "\n";
    return 1;
  }

  errs() << "LSP transport stopped normally\n";

  return 0;
}