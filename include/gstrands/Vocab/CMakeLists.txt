AddGameplayStrandsLibPubIncDir(Vocab)

set(LLVM_TARGET_DEFINITIONS Tokens.td)

tablegen(LLVM TokenKinds.inc
        -gen-searchable-tables
        EXTRA_INCLUDES ${GAMEPLAYSTRANDS_TABLEGEN_INCLUDE_DIRS}
)

add_custom_target(GameplayStrandsVocabTableGen
        DEPENDS ${TABLEGEN_OUTPUT}
)

add_dependencies(GameplayStrandsVocab
        GameplayStrandsVocabTableGen
)

target_sources(GameplayStrandsVocab
        PUBLIC
        FILE_SET generated_headers TYPE HEADERS
        BASE_DIRS "${GameplayStrandsSDK_BINARY_DIR}/include"
        FILES "${CMAKE_CURRENT_BINARY_DIR}/TokenKinds.inc"
)
