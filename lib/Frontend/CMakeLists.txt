file(GLOB_RECURSE frontend_sources
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(GameplayStrandsFrontend STATIC
        ${frontend_sources}
)

llvm_update_compile_flags(GameplayStrandsFrontend)

add_library(GameplayStrands::Frontend ALIAS GameplayStrandsFrontend)

target_compile_features(GameplayStrandsFrontend
        PUBLIC cxx_std_20
)

target_link_libraries(GameplayStrandsFrontend
        PUBLIC GameplayStrands::Model
        PUBLIC GameplayStrands::LLVMSupport
)

set_target_properties(GameplayStrandsFrontend PROPERTIES
        EXPORT_NAME Frontend
        POSITION_INDEPENDENT_CODE ON
)

