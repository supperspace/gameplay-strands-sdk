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

option(GAMEPLAYSTRANDS_ENABLE_INSTALL
        "Generate SDK installation and package rules"
        ${PROJECT_IS_TOP_LEVEL}
)

set(GAMEPLAYSTRANDS_LLVM_VERSION "23.1.1")

if (NOT TARGET LLVMSupport)
    find_package(LLVM ${GAMEPLAYSTRANDS_LLVM_VERSION}
            EXACT REQUIRED CONFIG
    )
endif ()

set(GAMEPLAYSTRANDS_TABLEGEN_INCLUDE_DIRS
        "${GameplayStrandsSDK_SOURCE_DIR}/include"
        ${GAMEPLAYSTRANDS_LLVM_INCLUDE_DIRS}
)

include(cmake/GameplayStrandsLLVM.cmake)
include(cmake/GameplayStrandsTableGen.cmake)

add_subdirectory(utils/TableGen)
add_subdirectory(lib)
add_subdirectory(include)

if (GAMEPLAYSTRANDS_BUILD_COMPILER)
    add_subdirectory(tools)
endif ()

if (GAMEPLAYSTRANDS_ENABLE_INSTALL)
    include(cmake/GameplayStrandsInstall.cmake)

    install(TARGETS GameplayStrandsFrontend
            EXPORT GameplayStrandsTargets
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            FILE_SET generated_headers DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif ()