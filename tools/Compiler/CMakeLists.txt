add_executable(GameplayStrandsCompiler
    Compiler.cpp)

add_executable(GameplayStrands::Compiler ALIAS GameplayStrandsCompiler)

target_compile_features(GameplayStrandsCompiler
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsCompiler
        PRIVATE
        GameplayStrands::Frontend
        GameplayStrands::LLVMSupport
)

set_target_properties(GameplayStrandsCompiler
        PROPERTIES OUTPUT_NAME gs-compiler)