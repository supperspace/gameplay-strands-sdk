import org.jetbrains.intellij.platform.gradle.TestFrameworkType

import org.jetbrains.intellij.platform.gradle.tasks.PrepareSandboxTask
import org.jetbrains.intellij.platform.gradle.IntelliJPlatformType

val riderHome = providers.gradleProperty("gstrands.riderHome")
val testProject = providers.gradleProperty("gstrands.testProject")

intellijPlatformTesting {
    runIde.register("runRider") {
        if (riderHome.isPresent) {
            localPath.set(file(riderHome.get()))
        } else {
            type.set(IntelliJPlatformType.Rider)
            version.set("2026.2")
        }

        plugins {
            bundledPlugin("org.jetbrains.plugins.textmate")
        }

        task {
            testProject.orNull?.let { args(it) }
        }
    }
}
val textMateBundleDirectory = layout.projectDirectory.dir("../TextMate")

tasks.withType<PrepareSandboxTask>().configureEach {
    val bundleDestination = pluginName.map { "$it/textmate/gstrands" }

    from(textMateBundleDirectory) {
        into(bundleDestination)
    }
}

plugins {
    id("org.jetbrains.kotlin.jvm")
    id("org.jetbrains.changelog")
    id("org.jetbrains.intellij.platform")
}

repositories {
    mavenCentral()

    intellijPlatform {
        defaultRepositories()
    }
}

// Read more: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin.html
dependencies {
    testImplementation(libs.junit)

    // IntelliJ Platform Gradle Plugin Dependencies Extension - read more: https://plugins.jetbrains.com/docs/intellij/tools-intellij-platform-gradle-plugin-dependencies-extension.html
    intellijPlatform {
        intellijIdea("2026.1.4")
        testFramework(TestFrameworkType.Platform)

        bundledPlugins("org.jetbrains.plugins.textmate")
        // Add plugin dependencies for compilation here, for example:
        // bundledPlugin("com.intellij.java")
    }

}
