add_executable(myproject_app
    main.cpp
)

target_link_libraries(myproject_app
    PRIVATE
        myproject
)
