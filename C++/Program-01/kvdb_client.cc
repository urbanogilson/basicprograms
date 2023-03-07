#include <string>
#include <vector>

#include "client.h"
#include "tcp_socket.h"

int main(int argc, char **argv)
{
  int port = 1234; // TODO: GET from argv

  // LOG_S(INFO) << "kvdb client";

  auto socket = kvdb::TcpSocket();
  auto client = kvdb::Client(socket, port);

  std::vector<std::string> cmd;
  for (int i = 1; i < argc; ++i)
  {
    cmd.push_back(argv[i]);
  }

  client.SendRequest(cmd);
  client.ReadResponse();

  return 0;
}
