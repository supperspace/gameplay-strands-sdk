add_executable(GameplayStrandsCompiler
    Compiler.cpp)

add_executable(GameplayStrands::Compiler ALIAS GameplayStrandsCompiler)

target_compile_features(GameplayStrandsCompiler
        PUBLIC cxx_std_20
)
