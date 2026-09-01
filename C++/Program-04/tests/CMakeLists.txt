include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    URL_HASH SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    # SYSTEM stops our warning set firing on GoogleTest's headers.
    SYSTEM
    EXCLUDE_FROM_ALL
    # Use an installed GoogleTest if there is one, otherwise download.
    FIND_PACKAGE_ARGS NAMES GTest
)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(
    tictactoe_tests
    example_test.cpp
    board_test.cpp
    game_test.cpp
)

target_link_libraries(
    tictactoe_tests
    PRIVATE
        tictactoe::tictactoe
        GTest::gtest_main
)

enable_compiler_warnings(tictactoe_tests)

include(GoogleTest)

gtest_discover_tests(tictactoe_tests)
