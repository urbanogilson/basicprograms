#include "client.h"

namespace kvdb {

Client::Client(const int port) : _port(port) { OpenConnection(); }

Client::~Client() { Close(); };

int Client::Socket(void) {
  _fd = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);

  // LOG_IF_S(ERROR, _fd == -1) << "socket()";

  return _fd;
}

int Client::Connect(void) {
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = ntohs(_port),  // TODO: move to param
      .sin_addr = {.s_addr = ntohl(
                       INADDR_LOOPBACK)} /*127.0.0.1*/  // TODO: move to param
  };

  int code = connect(_fd, (const struct sockaddr *)&addr, sizeof(addr));

  // LOG_IF_S(ERROR, code == -1) << "connect()";

  return code;
}

void Client::OpenConnection(void) {
  Socket();
  Connect();
}

void Client::Close(void) {
  close(_fd);
  _fd = -1;
}

int Client::ReadResponse(void) {
  // LOG_IF_S(ERROR, _fd == -1) << "invalid connection";

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
  // LOG_IF_S(ERROR, len < 4) << "bad response";
  // LOG_IF_S(ERROR, len > _K_MAX_MSG) << "too long";

  err = ReadFull(_fd, &rbuf[4], len);
  // LOG_IF_S(ERROR, err == -1) << "too long";

  int rescode = 0;
  memcpy(&rescode, &rbuf[4], 4);

  auto response = std::string(&rbuf[8], len - 4);
  std::cout << "server says: [" << rescode << "] " << response << std::endl;

  return 0;
}

int Client::SendRequest(const std::vector<std::string> &cmd) {
  // LOG_IF_S(ERROR, _fd == -1) << "invalid connection";

  int len = 4;
  for (auto &s : cmd) len += 4 + s.size();

  // LOG_IF_S(ERROR, len > _K_MAX_MSG) << "Key and value reached max size: " <<
  // _K_MAX_MSG;

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
}  // namespace kvdb