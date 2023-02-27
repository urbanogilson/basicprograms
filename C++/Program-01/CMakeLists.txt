cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package (glog 0.6.0 REQUIRED)
# find_package (GTest CONFIG REQUIRED)

project(db)

add_library(kvdb kvdb.cc)

add_library(server server.cc)

# add_library(threadpool threadpool.cc)

add_library(status status.cc)

target_link_libraries(server status glog::glog)

add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} server pthread)

add_executable(client client.cc)
target_link_libraries(client server glog::glog)


add_executable(${PROJECT_NAME}_test test.cc)
target_link_libraries(${PROJECT_NAME}_test gtest pthread)