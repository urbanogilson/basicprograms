#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "network.h"

struct args {
  char *port;
  int backlog;
};

void sigchld_handler(int s);

struct addrinfo hints_(void);

int bind_(struct addrinfo *servinfo);

void listen_(int sockfd, int backlog);

struct args args_(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stdout, "usage: port\nk");
    exit(1);
  }

  struct args input = {.port = argv[1], .backlog = 10};
  return input;
}

int main(int argc, char *argv[]) {
  struct args input = args_(argc, argv);
  int new_fd;
  struct addrinfo hints = hints_(), *servinfo;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  struct sigaction sa;
  char s[INET6_ADDRSTRLEN];
  int rv;

  if ((rv = getaddrinfo(NULL, input.port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  int sockfd = bind_(servinfo);

  listen_(sockfd, input.backlog);

  sa.sa_handler = sigchld_handler;  // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while (1) {  // main accept() loop
    sin_size = sizeof their_addr;

    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);
    printf("server: got connection from %s\n", s);

    if (!fork()) {    // this is the child process
      close(sockfd);  // child doesn't need the listener
      if (send(new_fd, "Hello, world!", 13, 0) == -1) perror("send");
      close(new_fd);
      exit(0);
    }
    close(new_fd);  // parent doesn't need this
  }

  return 0;
}

void sigchld_handler(int s) {
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0) {
  }

  errno = saved_errno;
}

struct addrinfo hints_() {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // use my IP
  return hints;
}

int bind_(struct addrinfo *servinfo) {
  int sockfd, yes = 1;
  struct addrinfo *p;

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);  // all done with this structure

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  return sockfd;
}

void listen_(int sockfd, int backlog) {
  if (listen(sockfd, backlog) == -1) {
    perror("listen");
    exit(1);
  }
}