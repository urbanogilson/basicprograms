cmake_minimum_required(VERSION 3.28)

project(tictactoe
    VERSION 0.1.0
    DESCRIPTION "A small modern C++ project template"
    LANGUAGES CXX
)

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR
        "tictactoe supports GCC and Clang only (got ${CMAKE_CXX_COMPILER_ID})")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options live in these modules, next to the code that acts on them.
include(CompilerWarnings)   # TICTACTOE_WARNINGS_AS_ERRORS + enable_compiler_warnings()
include(Sanitizers)         # TICTACTOE_ENABLE_SANITIZERS
include(Coverage)           # TICTACTOE_ENABLE_COVERAGE
include(StaticAnalysis)     # TICTACTOE_ENABLE_CLANG_TIDY + enable_clang_tidy()

# Opt-in, so `--preset dev` never downloads Google Benchmark to not use it.
option(TICTACTOE_ENABLE_BENCHMARKS "Build the Google Benchmark suite" OFF)

# Directory-scope instrumentation must be applied before any target is defined.
tictactoe_apply_sanitizers()
tictactoe_apply_coverage()

add_subdirectory(src)
add_subdirectory(app)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()

    if(TICTACTOE_ENABLE_BENCHMARKS)
        add_subdirectory(benchmark)
    endif()

    tictactoe_add_coverage_target()

    # Install & export rules, so find_package(tictactoe) works after
    # `cmake --install`.
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS tictactoe
        EXPORT tictactoeTargets
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(TARGETS tictactoe_app RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    set(TICTACTOE_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/tictactoe)

    install(EXPORT tictactoeTargets
        FILE tictactoeTargets.cmake
        NAMESPACE tictactoe::
        DESTINATION ${TICTACTOE_INSTALL_CMAKEDIR}
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tictactoeConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/tictactoeConfig.cmake
        INSTALL_DESTINATION ${TICTACTOE_INSTALL_CMAKEDIR}
    )
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/tictactoeConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/tictactoeConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/tictactoeConfigVersion.cmake
        DESTINATION ${TICTACTOE_INSTALL_CMAKEDIR}
    )
endif()
