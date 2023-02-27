// #define NDEBUG 0
#include <arpa/inet.h>
#include <glog/logging.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <regex>
#include <string>

#include "kvdb.h"
#include "server.h"

class Client : public kvdb::Server {
 private:
  int _port;  // TODO: Move to config
  int _fd;
  static const size_t _K_MAX_MSG = 4096;  // TODO: Move to config

 public:
  Client(const int port) : _port(port), _fd(-1) { OpenConnection(); }

  ~Client() { Close(); };

  int GetFd(void) noexcept { return _fd; };

  int Socket(void) {
    _fd = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);

    CHECK_NE(_fd, -1) << "socket()";

    return _fd;
  }

  int Connect(void) {
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = ntohs(_port),  // TODO: move to param
        .sin_addr = {.s_addr = ntohl(
                         INADDR_LOOPBACK)} /*127.0.0.1*/  // TODO: move to param
    };

    int code = connect(_fd, (const struct sockaddr *)&addr, sizeof(addr));

    CHECK_NE(code, -1) << "connect()";

    return code;
  }

  void OpenConnection(void) {
    Socket();
    Connect();
  }

  void Close(void) {
    close(_fd);
    _fd = -1;
  }

  int ReadResponse(void) {
    CHECK_NE(_fd, -1) << "Invalid connection";

    char rbuf[4 + _K_MAX_MSG + 1];

    errno = 0;
    int err = ReadFull(_fd, rbuf, 4);
    if (err) {
      if (errno == 0)
        std::cerr << "EOF\n";
      else
        std::cerr << "read error ()\n";

      return err;
    }

    int len = 0;
    memcpy(&len, rbuf, 4);
    CHECK_GE(len, 4) << "Bad response";
    CHECK_LT(len, _K_MAX_MSG) << "Too long";

    err = ReadFull(_fd, &rbuf[4], len);
    CHECK_EQ(err, 0) << "Too long";

    int rescode = 0;
    memcpy(&rescode, &rbuf[4], 4);

    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
  }

  int SendRequest(const std::vector<std::string> &cmd) {
    CHECK_NE(_fd, -1) << "Invalid connection";

    int len = 4;
    for (auto &s : cmd) len += 4 + s.size();

    CHECK_LT(len, _K_MAX_MSG)
        << "Key and value reached max size: " << _K_MAX_MSG;

    // 4 bytes header
    char wbuf[4 + _K_MAX_MSG];
    memcpy(wbuf, &len, 4);
    int n = cmd.size();
    memcpy(&wbuf[4], &n, 4);

    // body
    size_t cursor = 8;
    for (auto &s : cmd) {
      int size = s.size();
      memcpy(&wbuf[cursor], &size, 4);
      memcpy(&wbuf[cursor + 4], s.data(), size);
      cursor += 4 + size;
    }

    return WriteFull(_fd, wbuf, 4 + len);
  }
};

int main(int argc, char const *argv[]) {
  FLAGS_logtostdout = true;
  FLAGS_colorlogtostdout = true;
  google::InitGoogleLogging(argv[0]);

  LOG(INFO) << "kvdb client";

  auto client = Client(1234);

  std::vector<std::string> cmd;
  for (int i = 1; i < argc; ++i) {
    cmd.push_back(argv[i]);
  }

  client.SendRequest(cmd);
  client.ReadResponse();

  return 0;
}
