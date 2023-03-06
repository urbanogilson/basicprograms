#ifndef KVDB_KVDB_
#define KVDB_KVDB_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <iostream>
#include <string>

namespace kvdb {

enum class Result {
  OK,
  ERROR,
  NX,
};

class Kvdb {
 public:
  Kvdb(void) noexcept : name_("kvdb") {}
  Kvdb(const std::string &name) noexcept : name_(name) {}
  ~Kvdb() noexcept {}
  static int WriteFull(const int fd, char *buf, size_t n);
  static int ReadFull(const int fd, char *buf, size_t n);

 private:
  std::string name_;
};
}  // namespace kvdb

#endif  // KVDB_KVDB_