cmake_minimum_required (VERSION 3.22)

project (example1)

file(GLOB_RECURSE CHAPTER_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable (${PROJECT_NAME} ${CHAPTER_SRC_FILES})

target_link_libraries (${PROJECT_NAME} GTest::gtest)