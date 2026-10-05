AddGameplayStrandsLibrary(Frontend)

target_link_libraries(GameplayStrandsFrontend
        PUBLIC
        GameplayStrands::Basic
        GameplayStrands::AST
        GameplayStrands::Model
        PRIVATE
        GameplayStrands::Parse
        GameplayStrands::Project
        GameplayStrands::Lex
        GameplayStrands::Sema
)
