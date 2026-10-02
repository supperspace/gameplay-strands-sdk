package com.supperspace.gameplaystrands

import com.intellij.openapi.components.service

import com.intellij.openapi.options.BoundConfigurable
import com.intellij.openapi.ui.DialogPanel
import com.intellij.ui.dsl.builder.bindText
import com.intellij.ui.dsl.builder.panel

class StrandsSettingsConfigurable: BoundConfigurable("GameplayStrands") {
    override fun createPanel(): DialogPanel {
        val settings = service<StrandsSettings>()

        return panel {
            row("Language server executable:") {
                textField()
                    .bindText(settings::serverExecutablePath)
                    .resizableColumn()
            }
        }
    }

}