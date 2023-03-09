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

TEST(Client, init)
{
  MockSocket mock_socket;
  EXPECT_CALL(mock_socket, Socket(AF_INET, SOCK_STREAM, PF_UNSPEC))
      .Times(2)
      .WillOnce(Return(3))
      .WillOnce(Return(-1));
  EXPECT_CALL(mock_socket, Connect(_, NotNull(), 16)).Times(2);
  EXPECT_CALL(mock_socket, Close(_)).Times(2);

  int port = 1234;
  kvdb::Client(mock_socket, port);
  kvdb::Client(mock_socket, port);
}

void YourFailureFunction()
{
  LOG(INFO) << "Testing";
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  // google::InstallFailureFunction((google::logging_fail_func_t)&YourFailureFunction);
  return RUN_ALL_TESTS();
}