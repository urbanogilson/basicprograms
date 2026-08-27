include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

# Prevent GoogleTest from overriding compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(
    myproject_tests
    example_test.cpp
)

target_link_libraries(
    myproject_tests
    PRIVATE
        myproject
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(myproject_tests)
