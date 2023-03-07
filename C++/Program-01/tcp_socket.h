#ifndef KVDB_TCP_SYS_SOCKET_H_
#define KVDB_TCP_SYS_SOCKET_H_

#include "sys_socket.h"

namespace kvdb
{
  class TcpSocket : public SysSocket
  {
  public:
    int Socket(int __domain, int __type, int __protocol) override
    {
      return socket(__domain, __type, __protocol);
    }

    int Connect(int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len) override
    {
      return connect(__fd, __addr, __len);
    }

    int Close(int __fd) override { return close(__fd); }
  };

} // namespace kvdb

#endif // KVDB_TCP_SYS_SOCKET_H_