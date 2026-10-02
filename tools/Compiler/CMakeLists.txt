add_executable(GameplayStrandsCompiler
    Compiler.cpp)
llvm_update_compile_flags(GameplayStrandsCompiler)

add_executable(GameplayStrands::Compiler ALIAS GameplayStrandsCompiler)

target_compile_features(GameplayStrandsCompiler
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsCompiler
        PRIVATE
        GameplayStrands::Frontend
        GameplayStrands::LLVMSupport
        GameplayStrands::LLVMSupportLSP
)

set_target_properties(GameplayStrandsCompiler
        PROPERTIES OUTPUT_NAME gs-compiler)