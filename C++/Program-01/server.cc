#include "server.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>

#include "kvdb.h"
#include "status.h"

namespace kvdb {
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

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != -1) {
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

  // Server: waiting for connections...
  while (1) {
    sin_size = sizeof(their_addr);
    new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_sockfd == -1) {
      continue;
    }

    inet_ntop(their_addr.ss_family, GetInAddr((struct sockaddr *)&their_addr),
              address, sizeof(address));

    // ! Add task
  }
  return Status::Ok();
}
}  // namespace kvdb
