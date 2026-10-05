add_executable(gscompiler
    gscompiler.cpp)

llvm_update_compile_flags(gscompiler)

add_executable(GameplayStrands::Compiler ALIAS gscompiler)

target_compile_features(gscompiler
        PUBLIC cxx_std_20
)

target_link_libraries(gscompiler
        PRIVATE
        GameplayStrands::LLVMSupport
        GameplayStrands::Model
        GameplayStrands::Frontend
        GameplayStrands::AST
        GameplayStrands::Project
)
