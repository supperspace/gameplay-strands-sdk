AddGameplayStrandsLibPubIncDir(Parse)

set(LLVM_TARGET_DEFINITIONS ${PROJECT_SOURCE_DIR}/include/gstrands/AST/DeclNodes.td)
tablegen(LLVM ParserPolicy.inc
        -gen-searchable-tables
        EXTRA_INCLUDES ${GAMEPLAYSTRANDS_TABLEGEN_INCLUDE_DIRS}
)

add_custom_target(GameplayStrandsParseTableGen
        DEPENDS ${TABLEGEN_OUTPUT}
)

add_dependencies(GameplayStrandsParse
        GameplayStrandsParseTableGen
)

target_sources(GameplayStrandsParse
        PUBLIC
        FILE_SET generated_headers TYPE HEADERS
        BASE_DIRS "${GameplayStrandsSDK_BINARY_DIR}/include"
        FILES "${CMAKE_CURRENT_BINARY_DIR}/ParserPolicy.inc"
)
