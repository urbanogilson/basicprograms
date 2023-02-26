#ifndef KVDB_SERVER_H_
#define KVDB_SERVER_H_

#include <arpa/inet.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace kvdb {
struct Connection;

enum class State;

enum class Result {
  OK,
  ERROR,
  NX,
};

static std::unordered_map<std::string, std::string> _map;

class Server {
 private:
  int _port;
  static const size_t _K_MAX_MSG = 4096;

 public:
  static const size_t K_MAX_MSG = 4096;
  Server(const int port);
  Server(void);
  void Start(void);
  void StartAsync(void);
  static void Die(const char *message);
  static void DoSomething(const int fd);
  static int OneRequest(const int fd);
  static void FdSetNonBlocking(const int fd);
  static void ConnectionPut(std::vector<Connection *> &fdConnections,
                            Connection *connection);
  static void ConnectionIo(Connection *connection);
  static int AcceptNewConnection(std::vector<Connection *> &fdConnections,
                                 const int fd);
  static int WriteFull(const int fd, char *buf, size_t n);
  static int ReadFull(const int fd, char *buf, size_t n);
  static void StateResponse(Connection *connection);
  static void StateRequest(Connection *connection);
  static bool TryFillBuffer(Connection *connection);
  static bool TryFlushBuffer(Connection *connection);
  static bool TryOneRequest(Connection *connection);
  static bool CmdIs(const std::string &word, const char *cmd);
  static int DoRequest(const char *request, int requestLength, int *rescode,
                       char *response, int *responseLength);
  static int ParseRequest(const char *data, size_t len,
                          std::vector<std::string> &out);
  static int DoGet(std::vector<std::string> &cmd, char *response,
                   int *responseLength);
  static int DoSet(std::vector<std::string> &cmd, char *response,
                   int *responseLength);
  static int DoDel(std::vector<std::string> &cmd, char *response,
                   int *responseLength);
  ~Server();
};

enum class State {
  REQUEST,
  RESPONSE,
  END,
};

struct Connection {
  int fd = -1;
  State state = State::REQUEST;
  size_t rbuf_size = 0;
  char rbuf[4 + Server::K_MAX_MSG];
  size_t wbuf_size = 0;
  size_t wbuf_sent = 0;
  char wbuf[4 + Server::K_MAX_MSG];
};

}  // namespace kvdb
#endif  // KVDB_SERVER_H_