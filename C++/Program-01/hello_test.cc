#include <gtest/gtest.h>

TEST(ServerTest, Connection) {
  EXPECT_STRNE("hello", "world");
  EXPECT_EQ(7 * 6, 42);
}