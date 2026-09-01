# Per-target warning flags.
#
# A function rather than a directory-wide add_compile_options(), because this
# warning set is strict enough that GoogleTest and Google Benchmark would drown
# our own findings in third-party noise. Call it on first-party targets only.
#
# The top-level CMakeLists.txt already rejects anything but GCC and Clang.

option(TICTACTOE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

function(enable_compiler_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
        -Wshadow -Wformat=2 -Wnull-dereference -Wdouble-promotion
        -Wimplicit-fallthrough -Wold-style-cast -Wcast-align -Wunused
        -Woverloaded-virtual -Wnon-virtual-dtor
    )
    if(TICTACTOE_WARNINGS_AS_ERRORS)
        target_compile_options(${target} PRIVATE -Werror)
    endif()
endfunction()
