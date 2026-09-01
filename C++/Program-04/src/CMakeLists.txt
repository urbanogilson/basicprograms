add_library(tictactoe
    example.cpp
    board.cpp
    game.cpp
)
add_library(tictactoe::tictactoe ALIAS tictactoe)

target_sources(tictactoe
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES ${PROJECT_SOURCE_DIR}/include/tictactoe/example.h
)

target_compile_features(tictactoe
    PUBLIC cxx_std_20
)

enable_compiler_warnings(tictactoe)
enable_clang_tidy(tictactoe)
