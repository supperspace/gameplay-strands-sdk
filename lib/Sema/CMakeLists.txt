AddGameplayStrandsLibrary(Sema)

target_link_libraries(GameplayStrandsSema
        PUBLIC
        GameplayStrands::LLVMSupport
        PRIVATE
        GameplayStrands::Model
)
