cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(design_patterns)

file(GLOB SOURCE_FILES **/*.cpp)

foreach(SOURCE_PATH ${SOURCE_FILES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE_PATH} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_PATH})
endforeach(SOURCE_PATH ${SOURCE_FILES})
