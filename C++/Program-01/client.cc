#include <iostream>
#include <string>

#include "kvdb.h"

int main(int argc, char const *argv[]) {
  std::cout << "kvdb client" << std::endl;

  kvdb::Kvdb db();

  return 0;
}
