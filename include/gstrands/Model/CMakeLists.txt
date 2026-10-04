file(GLOB_RECURSE model_public_headers
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "*.h"
)

target_sources(GameplayStrandsModel
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
        FILES ${model_public_headers}
)
