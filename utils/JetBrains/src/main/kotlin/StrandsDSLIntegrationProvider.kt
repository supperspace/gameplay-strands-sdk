package com.supperspace.gameplaystrands

import com.intellij.execution.configurations.GeneralCommandLine
import com.intellij.openapi.components.service
import com.intellij.openapi.project.Project
import com.intellij.openapi.vfs.VirtualFile
import com.intellij.platform.lsp.api.LspIntegrationProvider
import com.intellij.platform.lsp.api.ProjectWideLspClientDescriptor
import com.intellij.execution.ExecutionException
import com.intellij.execution.process.BaseProcessHandler
import com.intellij.execution.process.ProcessAdapter
import com.intellij.execution.process.ProcessEvent
import com.intellij.execution.process.ProcessOutputTypes
import com.intellij.openapi.util.Key
import com.intellij.platform.lsp.api.LspServerListener
import org.eclipse.lsp4j.InitializeResult


class StrandsDSLClientDescriptor(project: Project) : ProjectWideLspClientDescriptor(project, "Strands DSL") {

    override fun isSupportedFile(file: VirtualFile): Boolean {
        return file.extension == "gss";
    }

    override fun createCommandLine(): GeneralCommandLine {
        val settings = service<StrandsSettings>()
        return GeneralCommandLine(settings.serverExecutablePath)
    }

    override fun startServerProcess(): BaseProcessHandler<*> {
        val log = project.service<StrandsLog>()
        log.info("Launching GameplayStrands server")

        val handler = try {
            super.startServerProcess()
        } catch (e: ExecutionException) {
            log.error("Launch failed: ${e.message}")
            throw e
        }

        handler.addProcessListener(object : ProcessAdapter() {
            override fun startNotified(event: ProcessEvent) {
                log.info("Process started; PID ${handler.process.pid()}")
            }

            override fun onTextAvailable(
                event: ProcessEvent, outputType: Key<*>
            ) {
                if (outputType == ProcessOutputTypes.STDERR) {
                    log.stderr(event.text)
                }
            }

            override fun processTerminated(event: ProcessEvent) {
                log.info("Process exited with code ${event.exitCode}")
            }
        })

        return handler
    }

    override val lspServerListener = object : LspServerListener {
        override fun serverInitialized(params: InitializeResult) {
            project.service<StrandsLog>().info("LSP initialized; server ready")
        }

        override fun serverStopped(shutdownNormally: Boolean) {
            project.service<StrandsLog>().info("LSP stopped; normal shutdown: $shutdownNormally")
        }
    }
}

class StrandsDSLIntegrationProvider : LspIntegrationProvider {

    override fun fileOpened(
        project: Project, file: VirtualFile, clientStarter: LspIntegrationProvider.LspClientStarter
    ) {

        if (file.extension == "gss" && project.service<StrandsServerControl>().enabled) {
            clientStarter.ensureClientStarted(
                StrandsDSLClientDescriptor(project)
            )
        }
    }

}