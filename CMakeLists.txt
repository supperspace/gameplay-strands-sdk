cmake_minimum_required(VERSION 3.28)

project(GameplayStrandsSDK
        VERSION 0.1.0
        LANGUAGES C CXX
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(GAMEPLAYSTRANDS_BUILD_COMPILER
        "Build the GameplayStrands compiler"
        ${PROJECT_IS_TOP_LEVEL}
)

add_subdirectory(lib)

if(GAMEPLAYSTRANDS_BUILD_COMPILER)
    add_subdirectory(tools)
endif()

# Keep all package files together, relative to the installation prefix.
set(GAMEPLAYSTRANDS_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/GameplayStrands"
)

install(EXPORT GameplayStrandsTargets
    FILE GameplayStrandsTargets.cmake
    NAMESPACE GameplayStrands::
    DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
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

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/GameplayStrandsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/GameplayStrandsConfigVersion.cmake"
    DESTINATION "${GAMEPLAYSTRANDS_INSTALL_CMAKEDIR}"
)

install(FILES LICENSE
    DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/GameplayStrands"
)
