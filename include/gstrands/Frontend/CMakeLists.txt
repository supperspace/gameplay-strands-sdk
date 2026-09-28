file(GLOB_RECURSE frontend_public_headers
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "*.h"
)



target_sources(GameplayStrandsFrontend
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
        FILES ${frontend_public_headers}
)

add_subdirectory(AST)
add_subdirectory(Basic)
add_subdirectory(Lex)
add_subdirectory(Parse)
