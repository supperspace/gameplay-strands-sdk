# Keep all package files together, relative to the installation prefix.
set(GAMEPLAYSTRANDS_INSTALL_CMAKEDIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/GameplayStrands"
)

install(EXPORT GameplayStrandsCoreTargets
        FILE GameplayStrandsCoreTargets.cmake
        NAMESPACE GameplayStrands::
        DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
        COMPONENT Core
)

install(EXPORT GameplayStrandsCompilerTargets
        FILE GameplayStrandsCompilerTargets.cmake
        NAMESPACE GameplayStrands::
        DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
        COMPONENT Compiler
)


configure_package_config_file(
        cmake/GameplayStrandsConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/GameplayStrandsConfig.cmake"
        INSTALL_DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
)

# The initial SDK does not yet promise compatibility between releases.
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/GameplayStrandsConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY ExactVersion
)

install(FILES LICENSE
        DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/GameplayStrands"
        COMPONENT Core
)

install(FILES LICENSES/LLVM.txt
        DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/GameplayStrands/LICENSES"
        COMPONENT Compiler
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/GameplayStrandsConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/GameplayStrandsConfigVersion.cmake"
        DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
        COMPONENT Core
)

install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GameplayStrandsLLVM.cmake"
        DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
        COMPONENT Compiler
)