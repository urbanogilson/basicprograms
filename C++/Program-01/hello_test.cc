#include <gtest/gtest.h>

#include "kvdb.h"

TEST(ServerTest, Connection) {
  kvdb::Kvdb db("test");
  EXPECT_STREQ("test", db.GetName().c_str());
}