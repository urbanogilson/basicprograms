include(FetchContent)

FetchContent_Declare(
    benchmark
    URL https://github.com/google/benchmark/archive/refs/tags/v1.9.5.zip
    URL_HASH SHA256=68c9c65cee4864db42c3af9ff5b5cfa32ce1b01d9653136c5f4ff96e18a9b8f2
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    # SYSTEM stops our warning set firing on Benchmark's headers.
    SYSTEM
    EXCLUDE_FROM_ALL
    # Use an installed Google Benchmark if there is one, otherwise download.
    FIND_PACKAGE_ARGS NAMES benchmark
)

# Build the library and nothing else. Benchmark's own test suite wants a second
# copy of GoogleTest, and its -Werror fires on compilers newer than it knows.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(benchmark)

add_executable(
    tictactoe_benchmarks
    example_benchmark.cpp
)

target_link_libraries(
    tictactoe_benchmarks
    PRIVATE
        tictactoe::tictactoe
        benchmark::benchmark_main
)

enable_compiler_warnings(tictactoe_benchmarks)
