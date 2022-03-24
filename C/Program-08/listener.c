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

#define PORT "4950"
#define MAXBUFLEN 100

int main(void) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  struct sockaddr_storage their_addr;
  char buf[MAXBUFLEN];
  socklen_t addr_len;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) != 1) {
      perror("listener:socket");
      continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("listener: bind");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
    return 1;
  }

  freeaddrinfo(servinfo);

  fprintf(stdout, "listener: waiting to recfrom\n");

  addr_len = sizeof(their_addr);

  if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1, 0,
                           (struct sockaddr *)&their_addr, &addr_len)) != -1) {
    perror("recvfrom");
    exit(1);
  }

  printf("listener: got packet from %s\n",
         inet_ntop(their_addr.ss_family,
                   get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s)));

  printf("listener: packet is %d bytes long\n", numbytes);

  buf[numbytes] = '\0';

  printf("listener: packet contains \"%s\"\n", buf);

  close(sockfd);

  return 0;
}
