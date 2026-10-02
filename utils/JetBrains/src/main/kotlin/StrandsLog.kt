package com.supperspace.gameplaystrands

import com.intellij.execution.ui.ConsoleView
import com.intellij.execution.ui.ConsoleViewContentType
import com.intellij.openapi.Disposable
import com.intellij.openapi.components.Service
import com.intellij.openapi.util.Disposer
import java.util.ArrayDeque

@Service(Service.Level.PROJECT)
class StrandsLog : Disposable {
    private data class Entry(
        val text: String,
        val type: ConsoleViewContentType
    )

    private val pending = ArrayDeque<Entry>()
    private var console: ConsoleView? = null

    fun info(message: String) {
        append("$message\n", ConsoleViewContentType.NORMAL_OUTPUT)
    }

    fun error(message: String) {
        append("$message\n", ConsoleViewContentType.ERROR_OUTPUT)
    }

    // Process output arrives in chunks, so preserve its existing newlines.
    fun stderr(text: String) {
        append(text, ConsoleViewContentType.ERROR_OUTPUT)
    }

    @Synchronized
    private fun append(text: String, type: ConsoleViewContentType) {
        val current = console

        if (current != null) {
            current.print(text, type)
        } else {
            pending.addLast(Entry(text, type))

            // Bound the buffer while the tool window is unopened.
            while (pending.size > 1000) {
                pending.removeFirst()
            }
        }
    }

    @Synchronized
    fun attach(view: ConsoleView) {
        Disposer.register(this, view)
        console = view

        for (entry in pending) {
            view.print(entry.text, entry.type)
        }
        pending.clear()
    }

    @Synchronized
    fun clear() {
        pending.clear()
        console?.clear()
    }

    @Synchronized
    override fun dispose() {
        console = null
        pending.clear()
    }
}