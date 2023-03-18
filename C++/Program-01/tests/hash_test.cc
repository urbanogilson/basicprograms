#include "../hash.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::AtLeast;
using testing::NotNull;
using testing::Return;

TEST(Node, CompareTrue) {
  EXPECT_EQ(kvdb::Node{.key = std::string("0")},
            kvdb::Node{.key = std::string("0")});
}

TEST(Node, CompareFalse) {
  EXPECT_NE(kvdb::Node{.key = std::string("0")},
            kvdb::Node{.key = std::string("1")});
}

TEST(Hash, NotPowerOfTwoSize) {
  ASSERT_DEATH(kvdb::Hash(static_cast<size_t>(3)),
               "size must be a positive and power of two");
}

TEST(Hash, PowerOfTwoSize) {
  kvdb::Hash hash(2);
  EXPECT_EQ(hash.Size(), 0);
}

TEST(Hash, Insert) {
  kvdb::Hash hash(2);
  EXPECT_EQ(hash.Size(), 0);
  hash.Insert("x", "y");
  EXPECT_EQ(hash.Size(), 1);
}

TEST(Hash, Get) {
  kvdb::Hash hash(2);
  hash.Insert("x", "z");
  hash.Insert("y", "y");
  hash.Insert("z", "x");
  EXPECT_EQ(hash.Get("x").value_or("not found"), "z");
  EXPECT_EQ(hash.Get("y").value_or("not found"), "y");
  EXPECT_EQ(hash.Get("z").value_or("not found"), "x");
}

TEST(Hash, Delete) {
  kvdb::Hash hash(2);
  hash.Insert("x", "y");
  EXPECT_EQ(hash.Size(), 1);
  EXPECT_EQ(hash.Delete("x"), 1);
  EXPECT_EQ(hash.Size(), 0);
}

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}