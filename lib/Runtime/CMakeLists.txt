add_library(GameplayStrandsRuntime STATIC
        Runtime.cpp
)

add_library(GameplayStrands::Runtime ALIAS GameplayStrandsRuntime)

target_compile_features(GameplayStrandsRuntime
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsRuntime
        PUBLIC GameplayStrands::Model
)

set_target_properties(GameplayStrandsRuntime PROPERTIES
        EXPORT_NAME Runtime
        POSITION_INDEPENDENT_CODE ON
)

target_sources(GameplayStrandsRuntime
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
        FILES "${GameplayStrandsSDK_SOURCE_DIR}/include/GameplayStrands/Runtime/Runtime.h"
)

install(TARGETS GameplayStrandsRuntime
        EXPORT GameplayStrandsTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
