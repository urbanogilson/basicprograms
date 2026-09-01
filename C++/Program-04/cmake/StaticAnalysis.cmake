# clang-tidy driven from the build.
#
# A target property rather than the global CMAKE_CXX_CLANG_TIDY, which would
# also analyse the fetched GoogleTest sources we can't fix anyway.

option(TICTACTOE_ENABLE_CLANG_TIDY "Run clang-tidy as part of the build" OFF)

function(enable_clang_tidy target)
    if(NOT TICTACTOE_ENABLE_CLANG_TIDY)
        return()
    endif()

    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy clang-tidy-18 clang-tidy-17)
    if(NOT CLANG_TIDY_EXECUTABLE)
        message(WARNING "TICTACTOE_ENABLE_CLANG_TIDY is ON but clang-tidy was not found")
        return()
    endif()

    set_target_properties(${target} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};--extra-arg=-Wno-unknown-warning-option"
    )
endfunction()
