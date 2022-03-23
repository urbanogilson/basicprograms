#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "network.h"

#define MAXDATASIZE 100  // max number of bytes we can get at once

struct args {
  char *hostname;
  char *port;
};

struct addrinfo hints_() {
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  return hints;
}

struct args args_(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stdout, "usage: hostname port\nk");
    exit(1);
  }

  struct args input = {.hostname = argv[1], .port = argv[2]};
  return input;
}

int main(int argc, char *argv[]) {
  struct args input = args_(argc, argv);
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints = hints_(), *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  if ((rv = getaddrinfo(input.hostname, input.port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo);  // all done with this structure

  if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
    perror("recv");
    exit(1);
  }

  buf[numbytes] = '\0';

  printf("client: received '%s'\n", buf);

  close(sockfd);

  return 0;
}
