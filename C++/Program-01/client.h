#ifndef KVDB_CLIENT_H_
#define KVDB_CLIENT_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "kvdb.h"
#include "server.h"

namespace kvdb {
class Client : public kvdb::Kvdb {
 private:
  int _fd = -1;
  int _port;                       // TODO: Move to config
  const size_t _K_MAX_MSG = 4096;  // TODO: Move to config

 public:
  Client(const int port);
  ~Client(void);
  int Socket(void);
  void OpenConnection(void);
  void Close(void);
  int Connect(void);
  int ReadResponse(void);
  int SendRequest(const std::vector<std::string> &cmd);
};
}  // namespace kvdb
#endif  // KVDB_CLIENT_H_