package com.supperspace.gameplaystrands

import com.intellij.execution.filters.TextConsoleBuilderFactory
import com.intellij.openapi.components.service
import com.intellij.openapi.project.DumbAware
import com.intellij.openapi.project.Project
import com.intellij.openapi.wm.ToolWindow
import com.intellij.openapi.wm.ToolWindowFactory
import com.intellij.ui.content.ContentFactory
import java.awt.BorderLayout
import java.awt.FlowLayout
import javax.swing.JButton
import javax.swing.JPanel

class StrandsToolWindowFactory : ToolWindowFactory, DumbAware {
    override fun createToolWindowContent(
        project: Project,
        toolWindow: ToolWindow
    ) {
        val console = TextConsoleBuilderFactory.getInstance()
            .createBuilder(project)
            .console

        project.service<StrandsLog>().attach(console)

        val buttons = JPanel(FlowLayout(FlowLayout.LEADING))

        buttons.add(JButton("Start").apply {
            addActionListener {
                project.service<StrandsServerControl>().start()
            }
        })

        buttons.add(JButton("Stop").apply {
            addActionListener {
                project.service<StrandsServerControl>().stop()
            }
        })

        buttons.add(JButton("Clear").apply {
            addActionListener {
                project.service<StrandsLog>().clear()
            }
        })

        val panel = JPanel(BorderLayout())
        panel.add(buttons, BorderLayout.NORTH)
        panel.add(console.component, BorderLayout.CENTER)

        val content = ContentFactory.getInstance()
            .createContent(panel, null, false)

        toolWindow.contentManager.addContent(content)
    }
}