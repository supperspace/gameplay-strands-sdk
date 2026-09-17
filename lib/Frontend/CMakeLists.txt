add_library(GameplayStrandsFrontend STATIC
        Frontend.cpp
)

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

target_sources(GameplayStrandsFrontend
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
        FILES "${GameplayStrandsSDK_SOURCE_DIR}/include/GameplayStrands/Frontend/Frontend.h"
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
        -print-enums
        -class=TokenKind
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
        PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)