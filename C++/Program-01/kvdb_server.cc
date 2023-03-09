#include <iostream>
#include <string>

#include "server.h"

int main(int argc, char const *argv[])
{
  // parse inputs to create server

  std::cout << "kvdb" << std::endl;

  auto server = kvdb::Server(1234);
  server.StartAsync();

  return 0;
}