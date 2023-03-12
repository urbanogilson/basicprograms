#include "../hash.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::AtLeast;
using testing::NotNull;
using testing::Return;

TEST(Node, Default) {
  struct kvdb::Node node {};
  EXPECT_EQ(node.code, 0);
  EXPECT_EQ(node.next, nullptr);
}

TEST(Tab, Default) {
  struct kvdb::Tab tab {};
  EXPECT_EQ(tab.tab, nullptr);
  EXPECT_EQ(tab.mask, 0);
  EXPECT_EQ(tab.size, 0);
}

TEST(Hash, NotPowerOfTwoSize) {
  ASSERT_DEATH(kvdb::Hash(3), "size must be a positive and power of two");
}

TEST(Hash, PowerOfTwoSize) {
  size_t size = 2;
  kvdb::Hash hash(size);
  EXPECT_NE(hash.Tab(), nullptr);
  EXPECT_EQ(hash.Size(), 0);
  EXPECT_EQ(hash.Mask(), size - 1);
}

TEST(Hash, Insert) {
  size_t size = 2;
  kvdb::Hash hash(size);
  struct kvdb::Node _1 {
    .code = 0
  };
  struct kvdb::Node _2 {
    .code = 1
  };
  struct kvdb::Node _3 {
    .code = 2
  };

  EXPECT_EQ(hash.Size(), 0);
  hash.Insert(&_1);
  hash.Insert(&_2);
  hash.Insert(&_3);
  // EXPECT_EQ(hash.Size(), 2);

  hash.Traverse([](struct kvdb::Node *node) {
    LOG(INFO) << (node ? node->code : 0) << " -> ";
  });
}

int main(int argc, char **argv) {
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}