#ifndef KVDB_SERVER_H_
#define KVDB_SERVER_H_

#include <arpa/inet.h>

#include <string>

#include "status.h"

namespace kvdb {
class Server {
 public:
  Status Start(const std::string &dbname, const int &port);

 private:
  void *GetInAddr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
};

}  // namespace kvdb

#endif  // KVDB_SERVER_H_