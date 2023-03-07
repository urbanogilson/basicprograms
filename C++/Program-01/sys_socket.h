#ifndef KVDB_SOCKET_H_
#define KVDB_SOCKET_H_

#include <sys/socket.h>
#include <unistd.h>

namespace kvdb
{
  class SysSocket
  {
  public:
    virtual int Socket(int __domain, int __type, int __protocol) = 0;
    virtual int Connect(int __fd, __CONST_SOCKADDR_ARG __addr,
                        socklen_t __len) = 0;
    virtual int Close(int __fd) = 0;
  };
} // namespace kvdb

#endif // KVDB_SOCKET_H_