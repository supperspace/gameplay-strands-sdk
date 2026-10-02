package com.supperspace.gameplaystrands

import com.intellij.ide.plugins.PluginManagerCore
import com.intellij.openapi.extensions.PluginId
import org.jetbrains.plugins.textmate.api.TextMateBundleProvider
import org.jetbrains.plugins.textmate.api.TextMateBundleProvider.PluginBundle

class StrandsTextMateBundleProvider : TextMateBundleProvider {

    override fun getBundles(): List<PluginBundle> {
        val plugin = checkNotNull(
            PluginManagerCore.getPlugin(
                PluginId.getId(
                    "com.supperspace.gameplaystrands.gameplaystrands-jetbrains"
                )
            )
        )

        return listOf(
            PluginBundle(
                "Gameplay Strands DSL",
                plugin.pluginPath.resolve("textmate/gstrands")
            )
        )
    }
}