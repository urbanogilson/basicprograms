include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main gmock_main)

add_executable(client_test client_test.cc)
add_executable(hash_test hash_test.cc)

target_link_libraries(client_test
 PRIVATE
  GTest::GTest
  client
  kvdb
)

target_link_libraries(hash_test
 PRIVATE
  GTest::GTest
  hash
)

add_test(client_gtests client_test hash_test)