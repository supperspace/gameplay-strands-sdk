add_executable(gscompilerd
        gscompilerd.cpp)

llvm_update_compile_flags(gscompilerd)

add_executable(GameplayStrands::CompilerDaemon ALIAS gscompilerd)

target_compile_features(gscompilerd
        PUBLIC cxx_std_20
)

target_link_libraries(gscompilerd
        PRIVATE
        GameplayStrands::LLVMSupport
        GameplayStrands::LLVMSupportLSP
        GameplayStrands::Model
        GameplayStrands::Frontend
        GameplayStrands::AST
        GameplayStrands::Project
)
