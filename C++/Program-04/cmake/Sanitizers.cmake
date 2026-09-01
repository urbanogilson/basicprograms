# ASan/UBSan, applied at directory scope.
#
# A whole-build switch rather than a per-target one, because instrumentation has
# to cover everything linked into one binary, GoogleTest included. Mixing an
# instrumented library with an uninstrumented dependency gives false
# container-overflow and ODR reports.

option(TICTACTOE_ENABLE_SANITIZERS "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

macro(tictactoe_apply_sanitizers)
    if(TICTACTOE_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=address,undefined)
        message(STATUS "Sanitizers enabled: address, undefined")
    endif()
endmacro()
