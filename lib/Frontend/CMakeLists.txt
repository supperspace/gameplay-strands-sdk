file(GLOB_RECURSE frontend_sources
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(GameplayStrandsFrontend STATIC
        ${frontend_sources}
)

llvm_update_compile_flags(GameplayStrandsFrontend)

add_library(GameplayStrands::Frontend ALIAS GameplayStrandsFrontend)

target_compile_features(GameplayStrandsFrontend
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsFrontend
        PUBLIC GameplayStrands::Model
        PUBLIC GameplayStrands::LLVMSupport
)

set_target_properties(GameplayStrandsFrontend PROPERTIES
        EXPORT_NAME Frontend
        POSITION_INDEPENDENT_CODE ON
)

file(GLOB_RECURSE frontend_public_headers
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "${GameplayStrandsSDK_SOURCE_DIR}/include/gstrands/Frontend/*.h"
)

target_sources(GameplayStrandsFrontend
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
        FILES ${frontend_public_headers}
)

if (GAMEPLAYSTRANDS_ENABLE_INSTALL)
    install(TARGETS GameplayStrandsFrontend
            EXPORT GameplayStrandsTargets
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif ()

set(LLVM_TARGET_DEFINITIONS Tokens.td)

tablegen(LLVM TokenKinds.inc
        -gen-searchable-tables
        EXTRA_INCLUDES ${GAMEPLAYSTRANDS_LLVM_INCLUDE_DIRS}
)

add_custom_target(GameplayStrandsFrontendTableGen
        DEPENDS ${TABLEGEN_OUTPUT}
)

add_dependencies(GameplayStrandsFrontend
        GameplayStrandsFrontendTableGen
)

target_sources(GameplayStrandsFrontend
        PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/TokenKinds.inc"
)

target_include_directories(GameplayStrandsFrontend
        PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
)