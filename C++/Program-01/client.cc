#include <arpa/inet.h>
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
  int _port;
  static const size_t _K_MAX_MSG = 4096;

 public:
  Client(const int port) : _port(port) {}

  int Open(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      std::cerr << "socket()\n";
      return -1;
    }
    std::cout << "fd(" << fd << ")\n";

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = ntohs(_port),
        .sin_addr = {.s_addr = ntohl(INADDR_LOOPBACK)} /*127.0.0.1*/};

    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
      std::cout << "connect()\n";
      return -1;
    }

    return fd;
  }

  void Close(int fd) { close(fd); }

  static int ReadResponse(const int fd) {
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

    int rescode = 0;
    if (len < 4) {
      std::cerr << "bad response\n";
      return -1;
    }

    memcpy(&rescode, &rbuf[4], 4);

    printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[8]);
    return 0;
  }

  static int SendRequest(const int fd, const std::vector<std::string> &cmd) {
    int len = 4;

    for (auto &s : cmd) {
      len += 4 + s.size();
    }

    if (len > _K_MAX_MSG) {
      return -1;
    }

    // 4 bytes header
    char wbuf[4 + _K_MAX_MSG];
    memcpy(wbuf, &len, 4);
    int n = cmd.size();
    memcpy(&wbuf[4], &n, 4);

    size_t cur = 8;
    for (auto &s : cmd) {
      int p = s.size();
      memcpy(&wbuf[cur], &p, 4);
      memcpy(&wbuf[cur + 4], s.data(), s.size());
      cur += 4 + s.size();
    }
    return WriteFull(fd, wbuf, 4 + len);
  }
  ~Client(){};
};

int main(int argc, char const *argv[]) {
  std::cout << "kvdb client" << std::endl;

  auto client = Client(1234);

  int fd = client.Open();

  std::vector<std::string> cmd;
  for (int i = 1; i < argc; ++i) {
    std::cout << argv[i] << std::endl;
    cmd.push_back(argv[i]);
  }
  int32_t err = client.SendRequest(fd, cmd);
  if (err) {
    goto L_DONE;
  }
  err = client.ReadResponse(fd);
  if (err) {
    goto L_DONE;
  }

L_DONE:
  client.Close(fd);

  return 0;
}
