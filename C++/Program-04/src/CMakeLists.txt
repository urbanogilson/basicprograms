add_library(myproject
    example.cpp
)

target_include_directories(myproject
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(myproject
    PUBLIC cxx_std_20
)
