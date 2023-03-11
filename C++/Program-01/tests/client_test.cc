#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../sys_socket.h"
#include "../client.h"

class MockSocket : public kvdb::SysSocket
{
public:
  MOCK_METHOD(int, Socket, (int __domain, int __type, int __protocol), (override));
  MOCK_METHOD(int, Connect, (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len), (override));
  MOCK_METHOD(int, Close, (int __fd), (override));
};

using testing::_;
using testing::AtLeast;
using testing::NotNull;
using testing::Return;

TEST(Client, on_socket_success)
{
  int port = 1234;
  MockSocket mock_socket;
  EXPECT_CALL(mock_socket, Socket(AF_INET, SOCK_STREAM, PF_UNSPEC))
      .Times(1)
      .WillOnce(Return(2));
  EXPECT_CALL(mock_socket, Connect(_, NotNull(), 16)).Times(1);
  EXPECT_CALL(mock_socket, Close(_)).Times(1);
  kvdb::Client(mock_socket, port);
}

TEST(Client, on_socket_error)
{
  int port = 1234;
  MockSocket mock_socket;
  EXPECT_CALL(mock_socket, Socket(AF_INET, SOCK_STREAM, PF_UNSPEC)).WillRepeatedly(Return(-1));
  ASSERT_DEATH(kvdb::Client(mock_socket, port), "_fd != -1 \\(-1 vs\\. -1\\) socket\\(\\)");
}

int main(int argc, char **argv)
{
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}