#include <iostream>
#include <string>

#include "server.h"

int main(int argc, char const *argv[]) {
  // parse inputs to create server

  std::cout << "kvdb" << std::endl;

  kvdb::Server server;
  server.Start(std::string("kvdb"), 8080);

  return 0;
}
