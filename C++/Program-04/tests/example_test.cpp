#include "tictactoe/example.h"

#include <gtest/gtest.h>

TEST(AddTest, AddsTwoNumbers) {
    EXPECT_EQ(tictactoe::add(2, 3), 5);
}

TEST(AddTest, HandlesNegativeNumbers) {
    EXPECT_EQ(tictactoe::add(-2, 3), 1);
}
