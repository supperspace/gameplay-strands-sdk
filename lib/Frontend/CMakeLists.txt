add_library(GameplayStrandsFrontend STATIC
        Frontend.cpp
)

add_library(GameplayStrands::Frontend ALIAS GameplayStrandsFrontend)

target_compile_features(GameplayStrandsFrontend
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsFrontend
        PUBLIC GameplayStrands::Model
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

install(TARGETS GameplayStrandsFrontend
        EXPORT GameplayStrandsTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
