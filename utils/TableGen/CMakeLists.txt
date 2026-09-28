include(AddLLVM)
include(TableGen)

set(LLVM_LINK_COMPONENTS Support)

add_tablegen(gs-tablegen GAMEPLAYSTRANDS
        DISABLE_PCH_REUSE
        TableGen.cpp
        ASTNodesEmitter.cpp
)

set_target_properties(gs-tablegen PROPERTIES
        CXX_SCAN_FOR_MODULES OFF
)

target_link_libraries(gs-tablegen
        PRIVATE GameplayStrands::LLVMSupport
)