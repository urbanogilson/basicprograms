cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(kvdb VERSION 0.0.1)

add_executable(kvdb main.cc)

add_library(server server.cc)

add_library(status status.cc)

