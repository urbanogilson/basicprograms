add_executable(tictactoe_app
    main.cpp
)

target_link_libraries(tictactoe_app
    PRIVATE
        tictactoe::tictactoe
)

enable_compiler_warnings(tictactoe_app)
enable_clang_tidy(tictactoe_app)
