# LLVM provides these differently in its source build and package config.
if(LLVM_INCLUDE_DIRS)
    set(GAMEPLAYSTRANDS_LLVM_INCLUDE_DIRS
            ${LLVM_INCLUDE_DIRS}
    )
else()
    set(GAMEPLAYSTRANDS_LLVM_INCLUDE_DIRS
            "${LLVM_MAIN_INCLUDE_DIR}"
            "${LLVM_INCLUDE_DIR}"
    )
endif()

if(NOT TARGET GameplayStrands::LLVMSupport)
    add_library(GameplayStrands::LLVMSupport INTERFACE IMPORTED)

    target_include_directories(GameplayStrands::LLVMSupport
            SYSTEM INTERFACE ${GAMEPLAYSTRANDS_LLVM_INCLUDE_DIRS}
    )

    separate_arguments(
            gameplay_strands_llvm_definitions
            NATIVE_COMMAND "${LLVM_DEFINITIONS}"
    )

    list(TRANSFORM gameplay_strands_llvm_definitions
            REPLACE "^-D" ""
    )

    target_compile_definitions(GameplayStrands::LLVMSupport
            INTERFACE ${gameplay_strands_llvm_definitions}
    )

    target_link_libraries(GameplayStrands::LLVMSupport
            INTERFACE LLVMSupport
    )
endif()