set(LLVM_TARGET_DEFINITIONS Diagnostics.td)
tablegen(LLVM DiagnosticDefs.inc
        -gen-searchable-tables
        EXTRA_INCLUDES ${GAMEPLAYSTRANDS_TABLEGEN_INCLUDE_DIRS}
)

add_custom_target(GameplayStrandsFrontendBasicTableGen
        DEPENDS ${TABLEGEN_OUTPUT}
)

add_dependencies(GameplayStrandsBasic
        GameplayStrandsFrontendBasicTableGen
)

target_sources(GameplayStrandsBasic
        PUBLIC
        FILE_SET generated_headers TYPE HEADERS
        BASE_DIRS "${GameplayStrandsSDK_BINARY_DIR}/include"
        FILES "${CMAKE_CURRENT_BINARY_DIR}/DiagnosticDefs.inc"
)

AddGameplayStrandsLibPubIncDir(Basic)
