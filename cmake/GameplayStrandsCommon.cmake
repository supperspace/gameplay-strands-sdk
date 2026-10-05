function(GameplayStrandsDisableExceptions Target)
    if(MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # MSVC and clang-cl
        target_compile_options(${Target} PRIVATE
                "$<$<COMPILE_LANGUAGE:CXX>:/EHs-c->"
        )
        target_compile_definitions(${Target} PRIVATE
                "$<$<COMPILE_LANGUAGE:CXX>:_HAS_EXCEPTIONS=0>"
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        target_compile_options(${Target} PRIVATE
                "$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>"
        )
    else()
        message(FATAL_ERROR
                "Exception flags unavailable for ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

function(AddGameplayStrandsLibrary LibName)
    cmake_parse_arguments(ARG "LLVM_FREE" "" "" ${ARGN})

    file(GLOB_RECURSE GSTRANDS_${LibName}_SRCS
            CONFIGURE_DEPENDS
            LIST_DIRECTORIES false
            "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )

    add_library(GameplayStrands${LibName} STATIC
            ${GSTRANDS_${LibName}_SRCS}
    )

    if(ARG_LLVM_FREE)
        set(Distribution Core)
        GameplayStrandsDisableExceptions(GameplayStrands${LibName})
    else()
        set(Distribution Compiler)
        llvm_update_compile_flags(GameplayStrands${LibName})
    endif()

    add_library(GameplayStrands::${LibName} ALIAS GameplayStrands${LibName})

    target_compile_features(GameplayStrands${LibName}
            PUBLIC cxx_std_20
    )

    set_target_properties(GameplayStrands${LibName} PROPERTIES
            EXPORT_NAME ${LibName}
            POSITION_INDEPENDENT_CODE ON
            GAMEPLAYSTRANDS_DISTRIBUTION "${Distribution}"
    )

    set_property(GLOBAL APPEND
            PROPERTY GAMEPLAY_STRANDS_LIBRARIES "GameplayStrands${LibName}"
    )

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
