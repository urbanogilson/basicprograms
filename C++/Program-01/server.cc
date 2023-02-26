#include "server.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <regex>
#include <vector>

#include "kvdb.h"

namespace kvdb {

Server::Server(const int port) : _port(port) {}
Server::Server(void) : _port(1234) {}

void Server::StartAsync(void) {
  int val = 1;
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
    Die("setsockopt()");
    return;
  }

  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = ntohs(_port),
      .sin_addr = {.s_addr = ntohl(0) /*wildcard address 0.0.0.0*/}};

  int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
  if (rv) {
    Die("bind()");
    return;
  }

  rv = listen(fd, SOMAXCONN);
  if (rv) {
    Die("listen()");
    return;
  }

  // a map of all client connections, keyed by fd
  std::vector<Connection *> fdConnections;

  // set the listen fd to nonblocking mode
  FdSetNonBlocking(fd);
  std::vector<struct pollfd> pollArgs;

  for (;;) {
    // prepare the arguments of the poll()
    pollArgs.clear();
    // for convenience, the listening fd is put in the first position
    struct pollfd pfd = {.fd = fd, .events = POLLIN, .revents = 0};
    pollArgs.push_back(pfd);

    // connection fds
    for (Connection *conn : fdConnections) {
      if (!conn) {
        continue;
      };

      struct pollfd pfd = {
          .fd = conn->fd,
          .events =
              (short int)((conn->state == State::REQUEST ? POLLIN : POLLOUT) |
                          POLLERR),
          .revents = 0};
      pollArgs.push_back(pfd);
    }

    // poll for active fds
    // the timeout argument doesn't matter here
    int rv = poll(pollArgs.data(), (nfds_t)pollArgs.size(), 1000);
    if (rv < 0) {
      Die("poll");
    }

    // process active connections
    for (size_t i = 1; i < pollArgs.size(); i++) {
      if (pollArgs[i].revents) {
        auto *conn = fdConnections[pollArgs[i].fd];
        ConnectionIo(conn);
        if (conn->state == State::END) {
          // client closed normally, or something bad happened.
          // destroy this connection
          fdConnections[conn->fd] = NULL;
          (void)close(conn->fd);
          free(conn);
        }
      }
    }

    // try to accept a new connection if the listening fd is active
    if (pollArgs[0].revents) {
      (void)AcceptNewConnection(fdConnections, fd);
    }
  }
}

int Server::ReadFull(const int fd, char *buf, size_t n) {
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
int Server::WriteFull(const int fd, char *buf, size_t n) {
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

int Server::OneRequest(const int fd) {
  char rbuf[4 + _K_MAX_MSG + 1];
  errno = 0;

  int err = ReadFull(fd, rbuf, 4);
  if (err) {
    if (errno == 0)
      std::cerr << "EOF\n";
    else
      std::cerr << "read error ()\n";

    return err;
  }
  int len = 0;
  memcpy(&len, rbuf, 4);
  if (len > _K_MAX_MSG) {
    std::cerr << "too long\n";
    return -1;
  }

  err = ReadFull(fd, &rbuf[4], len);
  if (err) {
    std::cerr << "read error ()\n";
    return err;
  }

  rbuf[4 + len] = '\0';
  std::cout << "Client says: " << &rbuf[4] << std::endl;

  // reply
  const char reply[] = "world";
  char wbuf[4 + sizeof(reply)];
  len = (int)strlen(reply);

  memcpy(wbuf, &len, 4);
  memcpy(&wbuf[4], reply, len);

  return write(fd, wbuf, 4 + len);
}

void Server::DoSomething(const int fd) {
  char rbuf[64] = {};

  ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);

  if (n < 0) {
    std::cout << "read() error\n";
  }
  std::cout << "client says:" << rbuf << std::endl;

  char wbuf[] = "hello world!";
  write(fd, wbuf, strlen(wbuf));
}

void Server::FdSetNonBlocking(const int fd) {
  errno = 0;
  int flags = fcntl(fd, F_GETFL, 0);
  if (errno) {
    Die("fcntl error");
    return;
  }

  flags |= O_NONBLOCK;

  errno = 0;
  (void)fcntl(fd, F_SETFL, flags);
  if (errno) {
    Die("fcntl error");
  }
}

void Server::Die(const char *message) {
  std::cerr << "Die: " << message << std::endl;
}

void Server::ConnectionPut(std::vector<Connection *> &fdConnections,
                           Connection *connection) {
  if (fdConnections.size() <= (size_t)connection->fd) {
    fdConnections.resize(connection->fd + 1);
  }
  fdConnections[connection->fd] = connection;
}

int Server::AcceptNewConnection(std::vector<Connection *> &fdConnections,
                                const int fd) {
  struct sockaddr_in client_addr;
  socklen_t socklen = sizeof(client_addr);
  int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);

  if (connfd < 0) {
    Die("accept error()");
    return -1;  // erro
  }

  // set the new connection fd to nonblocking mode
  FdSetNonBlocking(connfd);

  // creating the struct Conn
  Connection *conn = (Connection *)malloc(sizeof(Connection));
  if (!conn) {
    Die("malloc Connection");
    close(connfd);
    return -1;
  }

  conn->fd = connfd;
  conn->state = State::REQUEST;
  conn->rbuf_size = 0;
  conn->wbuf_size = 0;
  conn->wbuf_sent = 0;

  ConnectionPut(fdConnections, conn);
  return 0;
}
void Server::ConnectionIo(Connection *connection) {
  if (connection->state == State::REQUEST)
    StateRequest(connection);
  else if (connection->state == State::RESPONSE)
    StateResponse(connection);
  else
    assert(0);  // not expected
}

void Server::StateResponse(Connection *connection) {
  while (TryFlushBuffer(connection)) {
  }
}

void Server::StateRequest(Connection *connection) {
  while (TryFillBuffer(connection)) {
  }
}

bool Server::TryFillBuffer(Connection *connection) {
  assert(connection->rbuf_size < sizeof(connection->rbuf));
  ssize_t rv = 0;

  do {
    size_t cap = sizeof(connection->rbuf) - connection->rbuf_size;
    rv = read(connection->fd, &connection->rbuf[connection->rbuf_size], cap);
  } while (rv < 0 && errno == EINTR);

  if (rv < 0 && errno == EAGAIN) return false;

  if (rv < 0) {
    Die("read() error");
    connection->state = State::END;
    return false;
  }

  if (rv == 0) {
    if (connection->rbuf_size > 0) {
      Die("unexpected EOF");
    } else {
      Die("EOF");
    }
    connection->state = State::END;
    return false;
  }

  connection->rbuf_size += (size_t)rv;
  assert(connection->rbuf_size <=
         sizeof(connection->rbuf) - connection->rbuf_size);

  while (TryOneRequest(connection)) {
  }

  return connection->state == State::REQUEST;
}

bool Server::TryFlushBuffer(Connection *connection) {
  ssize_t rv = 0;
  do {
    size_t remain = connection->wbuf_size - connection->wbuf_sent;
    rv =
        write(connection->fd, &connection->wbuf[connection->wbuf_sent], remain);
  } while (rv < 0 && errno == EINTR);
  if (rv < 0 && errno == EAGAIN) {
    // got EAGAIN, stop.
    return false;
  }
  if (rv < 0) {
    Die("write() error");
    connection->state = State::END;
    return false;
  }
  connection->wbuf_sent += (size_t)rv;
  assert(connection->wbuf_sent <= connection->wbuf_size);
  if (connection->wbuf_sent == connection->wbuf_size) {
    // response was fully sent, change state back
    connection->state = State::REQUEST;
    connection->wbuf_sent = 0;
    connection->wbuf_size = 0;
    return false;
  }
  // still got some data in wbuf, could try to write again
  return true;
}

bool Server::TryOneRequest(Connection *connection) {
  if (connection->rbuf_size < 4) {
    return false;
  }

  int len = 0;
  memcpy(&len, &connection->rbuf[0], 4);
  if (len > _K_MAX_MSG) {
    Die("too long");
    connection->state = State::END;
    return false;
  }

  if (4 + len > connection->rbuf_size) {
    // not enough data in the buffer. Will retry in the next iteration
    return false;
  }

  // got one request, generate the response
  int rescode = 0;
  int wlen = 0;
  int err = DoRequest(&connection->rbuf[4], len, &rescode,
                      &connection->wbuf[4 + 4], &wlen);
  std::cout << "DoRequest=" << err << "wlen=" << wlen << " \n";

  if (err) {
    connection->state = State::END;
    return false;
  }

  // generating echoing response
  wlen += 4;
  memcpy(&connection->wbuf[0], &wlen, 4);
  memcpy(&connection->wbuf[4], &rescode, 4);
  connection->wbuf_size = 4 + wlen;

  // TODO remove the request from the buffer.
  // note: frequent memmove is inefficient.
  // note: need better handling for production code.
  size_t remain = connection->rbuf_size - 4 - len;
  if (remain) {
    memmove(connection->rbuf, &connection->rbuf[4 + len], remain);
  }
  connection->rbuf_size = remain;

  // change state
  connection->state = State::RESPONSE;
  StateResponse(connection);

  // continue the outer loop if the request was fully processed
  return (connection->state == State::REQUEST);
}

bool Server::CmdIs(const std::string &word, const char *cmd) {
  return 0 == strcasecmp(word.c_str(), cmd);
}

int Server::DoRequest(const char *request, int requestLength, int *rescode,
                      char *response, int *responseLength) {
  std::vector<std::string> cmd;
  if (0 != ParseRequest(request, requestLength, cmd)) {
    Die("bad request");
    return -1;
  }

  std::cout << "CMD: " << cmd[0] << " Size: " << cmd.size() << std::endl;

  if (cmd.size() == 2 && CmdIs(cmd[0], "get")) {
    *rescode = DoGet(cmd, response, responseLength);
  } else if (cmd.size() == 3 && CmdIs(cmd[0], "set")) {
    *rescode = DoSet(cmd, response, responseLength);
  } else if (cmd.size() == 2 && CmdIs(cmd[0], "del")) {
    *rescode = DoDel(cmd, response, responseLength);
  } else {
    *rescode = (int)Result::ERROR;
    const char *msg = "Uknown cmd";
    strcpy((char *)response, msg);
    *responseLength = strlen(msg);
  }
  return 0;
}

int Server::DoGet(std::vector<std::string> &cmd, char *response,
                  int *responseLength) {
  if (!_map.count(cmd[1])) return (int)Result::NX;  // TODO change return
  std::cout << "(GET) k: " << cmd[1] << std::endl;
  std::string &val = _map[cmd[1]];
  assert(val.size() <= _K_MAX_MSG);
  memcpy(response, val.data(), val.size());
  *responseLength = (int)val.size();
  return (int)Result::OK;
}

int Server::DoSet(std::vector<std::string> &cmd, char *response,
                  int *responseLength) {
  std::cout << "(SET) k: " << cmd[1] << " v: " << cmd[2] << std::endl;
  (void)response;
  (void)responseLength;
  _map[cmd[1]] = cmd[2];
  return (int)Result::OK;
}

int Server::DoDel(std::vector<std::string> &cmd, char *response,
                  int *responseLength) {
  std::cout << "(DEL) k: " << cmd[1] << std::endl;
  (void)response;
  (void)responseLength;
  _map.erase(cmd[1]);
  return (int)Result::OK;
}

int Server::ParseRequest(const char *data, size_t len,
                         std::vector<std::string> &out) {
  if (len < 4) {
    return -1;
  }

  int n = 0;

  memcpy(&n, &data[0], 4);

  if (n > _K_MAX_MSG) return -1;

  size_t pos = 4;

  while (n--) {
    if (pos + 4 > len) {
      return -1;
    }
    int sz = 0;
    memcpy(&sz, &data[pos], 4);
    if (pos + 4 + sz > len) {
      return -1;
    }

    out.push_back(std::string((char *)&data[pos + 4], sz));
    pos += 4 + sz;
  }

  if (pos != len) return -1;

  return 0;
}

Server::~Server() {}

}  // namespace kvdb
