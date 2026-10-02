package com.supperspace.gameplaystrands

import com.intellij.openapi.components.Service
import com.intellij.openapi.components.service
import com.intellij.openapi.project.Project
import com.intellij.platform.lsp.api.LspClientManager

@Service(Service.Level.PROJECT)
class StrandsServerControl(private val project: Project) {
    @Volatile
    var enabled: Boolean = true
        private set

    fun start() {
        enabled = true
        project.service<StrandsLog>().info("Start requested")

        LspClientManager.getInstance(project).ensureClientStarted(
            StrandsDSLIntegrationProvider::class.java,
            StrandsDSLClientDescriptor(project)
        )
    }

    fun stop() {
        enabled = false
        project.service<StrandsLog>().info("Stop requested")

        LspClientManager.getInstance(project).stopClients(
            StrandsDSLIntegrationProvider::class.java
        )
    }
}