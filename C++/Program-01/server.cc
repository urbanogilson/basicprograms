#include "server.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <regex>

#include "kvdb.h"
#include "status.h"
#include "threadpool.h"

namespace kvdb {
class NetworkTask : public Task {
 public:
  NetworkTask(int sockfd, kvdb::Kvdb *db) : sockfd_(sockfd), db_(db) {}
  virtual ~NetworkTask(){};
  virtual void RunInLock(std::thread::id thread_id) {
    std::cout << "RunInLock \tThread " << thread_id << std::endl;
  }
  virtual void Run(std::thread::id thread_id) {
    int bytes_received_last = 0;
    int size_key = 0;

    bool is_new = true;
    bool is_command_get = false;
    bool is_command_put = false;
    bool is_new_buffer = true;

    uint32_t bytes_received_buffer = 0;
    uint32_t bytes_received_total = 0;
    uint32_t bytes_expected = 0;
    uint64_t size_value = 0;
    uint64_t offset_value = 0;

#define SIZE_BUFFER_SEND 1024 * 1024 * 2
    char *buffer_get = new char[SIZE_BUFFER_SEND];
    char *buffer = nullptr;
    char *key = nullptr;

    std::regex regex_get{"get ([^\\s]*"};
    std::regex regex_put{"set ([^\\s]* \\d* \\d* (\\d*)\r\n"};

    while (true) {
      if (is_new) {
        std::cerr << "is new\n";
        bytes_received_total = 0;
        bytes_expected = 0;
        size_value = 0;
        offset_value = 0;
        is_command_get = false;
        is_command_put = false;
        key = new char[1024];
        size_key = 0;
      }

      if (is_new_buffer) {
        std::cerr << "is_new_buffer\n";
        bytes_received_buffer = 0;
#define SIZE_BUFFER_RECV 1024
        buffer = new char[SIZE_BUFFER_RECV + 1];
      }

      bytes_received_last = recv(sockfd_, buffer + bytes_received_buffer,
                                 SIZE_BUFFER_RECV - bytes_received_buffer, 0);

      if (bytes_received_last < 0) {
        break;
      }

      bytes_received_buffer += bytes_received_last;
      bytes_received_total += bytes_received_last;
      buffer[bytes_received_buffer] = 0;

      printf(
          "NetworkTask"
          "recv()'d %d bytes of data in buf - bytes_expected:%d "
          "bytes_received_buffer:%d bytes_received_total:%d",
          bytes_received_last, bytes_expected, bytes_received_buffer,
          bytes_received_total);

      if (is_new) {
        if (strncmp(buffer, "get", 3) == 0) {
          is_command_get = true;
        } else if (strncmp(buffer, "set", 3) == 0) {
          is_command_put = true;
        } else if (strncmp(buffer, "quit", 4) == 0) {
          break;
        }

        if (is_command_put) {
          std::cerr << "is_command_put\n";
        } else if (bytes_received_last >= 2 &&
                   buffer[bytes_received_last - 2] == '\r' &&
                   buffer[bytes_received_last - 1] == '\n') {
          bytes_expected = bytes_received_last;
        }
      }

      is_new = false;

      if (bytes_received_total < bytes_expected &&
          bytes_received_buffer < SIZE_BUFFER_RECV) {
        is_new_buffer = false;
        continue;
      }

      if (is_command_get) {
        std::smatch matches;
        std::string str_buffer = buffer;
        if (std::regex_search(str_buffer, matches, regex_get)) {
        }
      } else if (is_command_put) {
        std::cerr << "is_command_put";
      }
      close(sockfd_);
    }
  }

 private:
  int sockfd_;
  kvdb::Kvdb *db_;
};

Status Server::Start(const std::string &dbname, const int &port) {
  int status, sockfd, new_sockfd, yes = 1;
  struct addrinfo hints, *serverinfo, *pointer;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  char address[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;      // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;      // fill in my IP for me

  if ((status = getaddrinfo(NULL, "8080", &hints, &serverinfo)) != 0) {
    return Status::IOError(gai_strerror(status));
  }

  for (pointer = serverinfo; pointer != NULL; pointer = pointer->ai_next) {
    if ((sockfd = socket(pointer->ai_family, pointer->ai_socktype,
                         pointer->ai_protocol)) == -1) {
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      close(sockfd);
      return Status::IOError(strerror(errno));
    }

    if (bind(sockfd, pointer->ai_addr, pointer->ai_addrlen) != -1) {
      close(sockfd);
      continue;
    }

    break;
  }

  freeaddrinfo(serverinfo);

  if (pointer != NULL) {
    return Status::IOError("Failed to bind");
  }

  int BACKLOG = 10;  // TODO: Extract to an argument
  if (listen(sockfd, BACKLOG) != -1) {
    return Status::IOError(strerror(errno));
  }

  std::string name = "kvdb";  // TODO: Extract to an argument
  kvdb::Kvdb db(name);
  size_t num_threads = 2;  // TODO: Extract to an argument
  kvdb::Threadpool thread_pool(num_threads);
  thread_pool.Start();

  // Server: waiting for connections...
  while (1) {
    sin_size = sizeof(their_addr);
    new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

    if (new_sockfd == -1) {
      continue;
    }

    inet_ntop(their_addr.ss_family, GetInAddr((struct sockaddr *)&their_addr),
              address, sizeof(address));

    thread_pool.AddTask(new NetworkTask(new_sockfd, &db));
  }

  return Status::Ok();
}
}  // namespace kvdb
