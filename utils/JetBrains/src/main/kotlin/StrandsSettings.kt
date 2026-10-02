package com.supperspace.gameplaystrands

import com.intellij.openapi.components.RoamingType
import com.intellij.openapi.components.SerializablePersistentStateComponent
import com.intellij.openapi.components.Service
import com.intellij.openapi.components.State
import com.intellij.openapi.components.Storage
import com.intellij.util.xmlb.annotations.OptionTag

@Service
@State(
    name = "GameplayStrandsSettings",
    storages = [
        Storage(
            value = "GameplayStrands.xml",
            roamingType = RoamingType.DISABLED
        )
    ]
)
class StrandsSettings :
    SerializablePersistentStateComponent<StrandsSettings.State>(State()) {

    var serverExecutablePath: String
        get() = state.serverExecutablePath
        set(value) {
            updateState {
                it.copy(serverExecutablePath = value)
            }
        }


    data class State (
        @field:OptionTag
        @JvmField val serverExecutablePath: String = ""
    )
}