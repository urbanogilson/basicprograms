cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(db VERSION 0.0.1)

add_executable(db main.cc)

add_library(kvdb kvdb.cc)

add_library(server server.cc)

add_library(threadpool threadpool.cc)

add_library(status status.cc)

