set(LLVM_TARGET_DEFINITIONS DeclNodes.td)
tablegen(GAMEPLAYSTRANDS DeclNodes.inc
        -gen-ast-nodes
        EXTRA_INCLUDES ${GAMEPLAYSTRANDS_TABLEGEN_INCLUDE_DIRS}
)

add_custom_target(GameplayStrandsASTTableGen
        DEPENDS ${TABLEGEN_OUTPUT}
)

add_dependencies(GameplayStrandsAST
        GameplayStrandsASTTableGen
)

target_sources(GameplayStrandsAST
        PUBLIC
        FILE_SET generated_headers TYPE HEADERS
        BASE_DIRS "${GameplayStrandsSDK_BINARY_DIR}/include"
        FILES "${CMAKE_CURRENT_BINARY_DIR}/DeclNodes.inc"
)

AddGameplayStrandsLibPubIncDir(AST)
