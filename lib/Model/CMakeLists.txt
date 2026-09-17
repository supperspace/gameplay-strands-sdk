add_library(GameplayStrandsModel STATIC
        Model.cpp
)

add_library(GameplayStrands::Model ALIAS GameplayStrandsModel)

target_compile_features(GameplayStrandsModel
        PUBLIC cxx_std_20
)

set_target_properties(GameplayStrandsModel PROPERTIES
        EXPORT_NAME Model
        POSITION_INDEPENDENT_CODE ON
)

# The base directory defines the public #include root in the build tree.
target_sources(GameplayStrandsModel
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
        FILES "${GameplayStrandsSDK_SOURCE_DIR}/include/GameplayStrands/Model/Model.h"
)


if (GAMEPLAYSTRANDS_ENABLE_INSTALL)

    install(TARGETS GameplayStrandsModel
            EXPORT GameplayStrandsTargets
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif ()


target_link_libraries(GameplayStrandsModel
        PUBLIC GameplayStrands::LLVMSupport
)