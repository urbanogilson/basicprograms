# gcov coverage, at directory scope for the same reason as Sanitizers.cmake:
# instrumentation has to be consistent across the link.
#
# Adds a `coverage` target that runs ctest and writes a gcovr report to
# ${CMAKE_BINARY_DIR}/coverage/.

option(TICTACTOE_ENABLE_COVERAGE "Build with coverage instrumentation" OFF)

macro(tictactoe_apply_coverage)
    if(TICTACTOE_ENABLE_COVERAGE)
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
        message(STATUS "Coverage instrumentation enabled")
    endif()
endmacro()

function(tictactoe_add_coverage_target)
    if(NOT TICTACTOE_ENABLE_COVERAGE)
        return()
    endif()

    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
        # Without VERBATIM the shell eats the punctuation and reports a
        # syntax error instead of printing the instruction.
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E echo
                "gcovr not found. Install it with: pipx install gcovr"
            COMMAND ${CMAKE_COMMAND} -E false
            COMMENT "Coverage report unavailable"
            VERBATIM
        )
        message(STATUS "gcovr not found -- the 'coverage' target will report how to install it")
        return()
    endif()

    add_custom_target(coverage
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
        COMMAND ${GCOVR_EXECUTABLE}
            --root ${PROJECT_SOURCE_DIR}
            --exclude ${PROJECT_SOURCE_DIR}/tests
            --exclude ${CMAKE_BINARY_DIR}/_deps
            --exclude-unreachable-branches
            --print-summary
            --html-details ${CMAKE_BINARY_DIR}/coverage/index.html
            --xml ${CMAKE_BINARY_DIR}/coverage/coverage.xml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests and generating coverage report in ${CMAKE_BINARY_DIR}/coverage"
        VERBATIM
    )

    if(TARGET tictactoe_tests)
        add_dependencies(coverage tictactoe_tests)
    endif()
endfunction()
