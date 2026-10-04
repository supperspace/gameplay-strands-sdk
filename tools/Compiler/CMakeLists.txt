add_executable(GameplayStrandsCompilerTool
    Compiler.cpp)
llvm_update_compile_flags(GameplayStrandsCompilerTool)

add_executable(GameplayStrands::CompilerTool ALIAS GameplayStrandsCompilerTool)

target_compile_features(GameplayStrandsCompilerTool
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsCompilerTool
        PRIVATE
        GameplayStrands::LLVMSupport
        GameplayStrands::LLVMSupportLSP
        GameplayStrands::Model
        GameplayStrands::Frontend
        GameplayStrands::AST
        GameplayStrands::Project
)

set_target_properties(GameplayStrandsCompilerTool
        PROPERTIES OUTPUT_NAME gs-compiler)