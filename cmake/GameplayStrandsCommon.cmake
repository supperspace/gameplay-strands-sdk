function(AddGameplayStrandsLibrary LibName)

    file(GLOB_RECURSE GSTRANDS_${LibName}_SRCS
            CONFIGURE_DEPENDS
            LIST_DIRECTORIES false
            "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )

    add_library(GameplayStrands${LibName} STATIC
            ${GSTRANDS_${LibName}_SRCS}
    )

    llvm_update_compile_flags(GameplayStrands${LibName})

    add_library(GameplayStrands::${LibName} ALIAS GameplayStrands${LibName})

    target_compile_features(GameplayStrands${LibName}
            PUBLIC cxx_std_20
    )

    set_target_properties(GameplayStrands${LibName} PROPERTIES
            EXPORT_NAME ${LibName}
            POSITION_INDEPENDENT_CODE ON
    )

    set_property(GLOBAL APPEND PROPERTY GAMEPLAY_STRANDS_LIBRARIES
            "GameplayStrands${LibName}")

endfunction()

function(AddGameplayStrandsLibPubIncDir LibName)

    file(GLOB_RECURSE GSTRANDS_${LibName}_PUBLIC_HEADERS
            CONFIGURE_DEPENDS
            LIST_DIRECTORIES false
            "*.h"
    )

    target_sources(GameplayStrands${LibName}
            PUBLIC
            FILE_SET HEADERS
            BASE_DIRS "${GameplayStrandsSDK_SOURCE_DIR}/include"
            FILES ${GSTRANDS_${LibName}_PUBLIC_HEADERS}
    )

endfunction()
