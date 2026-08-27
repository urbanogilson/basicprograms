#include <gtest/gtest.h>

#include "myproject/example.h"

TEST(AddTest, AddsTwoNumbers)
{
    EXPECT_EQ(myproject::add(2, 3), 5);
}

TEST(AddTest, HandlesNegativeNumbers)
{
    EXPECT_EQ(myproject::add(-2, 3), 1);
}
