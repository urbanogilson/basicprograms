#include "kvdb.h"

namespace kvdb {

int Kvdb::ReadFull(const int fd, char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = read(fd, buf, n);
    if (rv <= 0) {
      std::cerr << "error, unexpected EOF\n";
      return -1;
    }

    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }

  return 0;
}
int Kvdb::WriteFull(const int fd, char *buf, size_t n) {
  while (n > 0) {
    ssize_t rv = write(fd, buf, n);

    if (rv <= 0) {
      std::cerr << "error write() " << rv << " \n";
      return -1;
    }

    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buf += rv;
  }

  return 0;
}

}  // namespace kvdb
