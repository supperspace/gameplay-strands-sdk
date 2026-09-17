list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)

# An LLVM source build already selects its TableGen executable.
# Preserve that choice, including LLVM's host-tool arrangements.
if(NOT LLVM_TABLEGEN_EXE)
    if(TARGET llvm-tblgen)
        set(LLVM_TABLEGEN_EXE "$<TARGET_FILE:llvm-tblgen>")
        set(LLVM_TABLEGEN_TARGET llvm-tblgen)
    else()
        find_program(GAMEPLAYSTRANDS_LLVM_TABLEGEN
                NAMES llvm-tblgen
                HINTS "${LLVM_TOOLS_BINARY_DIR}"
                NO_DEFAULT_PATH
                REQUIRED
        )

        set(LLVM_TABLEGEN_EXE
                "${GAMEPLAYSTRANDS_LLVM_TABLEGEN}"
        )
        set(LLVM_TABLEGEN_TARGET
                "${GAMEPLAYSTRANDS_LLVM_TABLEGEN}"
        )
    endif()
endif()