#include <string>
#include <vector>
#include <glog/logging.h>

#include "client.h"
#include "tcp_socket.h"

int main(int argc, char **argv)
{
  FLAGS_alsologtostderr = true;
  FLAGS_colorlogtostderr = true;
  google::InitGoogleLogging(argv[0]);

  int port = 1234; // TODO: GET from argv (Google flags?)
  LOG(INFO) << "Running on port " << port;

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
