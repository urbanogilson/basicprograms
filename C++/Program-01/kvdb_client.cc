#include <string>
#include <vector>

#include "client.h"

int main(int argc, char **argv) {
  int port = 1234;  // TODO: GET from argv

  // LOG_S(INFO) << "kvdb client";

  auto client = kvdb::Client(port);

  std::vector<std::string> cmd;
  for (int i = 1; i < argc; ++i) {
    cmd.push_back(argv[i]);
  }

  client.SendRequest(cmd);
  client.ReadResponse();

  return 0;
}
