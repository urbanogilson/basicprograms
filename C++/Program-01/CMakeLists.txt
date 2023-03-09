cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

project(kvdb)

include(FetchContent)

FetchContent_Declare(
  glog
  GIT_REPOSITORY https://github.com/google/glog.git
  GIT_TAG        v0.6.0
)
FetchContent_MakeAvailable(glog)
# include_directories(glog::glog)

# add_library(Glog::Glog INTERFACE IMPORTED)
# target_link_libraries(Glog::Glog INTERFACE glog)

add_library(kvdb kvdb.cc)

add_library(server server.cc)

add_library(client client.cc)

add_library(hash hash.cc)

add_library(status status.cc)
target_link_libraries(server status)

target_link_libraries(client glog::glog)

add_executable(${PROJECT_NAME}_server kvdb_server.cc)
target_link_libraries(${PROJECT_NAME}_server server kvdb hash)

add_executable(kvdb_client kvdb_client.cc)
target_link_libraries(kvdb_client client kvdb glog::glog)

enable_testing()

add_subdirectory(tests)