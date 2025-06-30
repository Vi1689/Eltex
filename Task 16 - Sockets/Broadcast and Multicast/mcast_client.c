#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  struct sockaddr_in client = {0};

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    perror("Error - socket\n");
    return 1;
  }

  struct ip_mreqn mreqn = {0};

  if (inet_aton("224.0.0.1", &mreqn.imr_multiaddr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  mreqn.imr_address.s_addr = INADDR_ANY;

  if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn)) ==
      -1) {
    perror("Error - setsockopt");
    return 1;
  }

  client.sin_family = AF_INET;
  client.sin_port = htons(7777);
  client.sin_addr.s_addr = INADDR_ANY;

  if ((bind(fd, (struct sockaddr *)&client, sizeof(client))) == -1) {
    perror("Error - bind");
    return 1;
  }

  char buf[100] = {'\000'};

  if ((recv(fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - recv");
    return 1;
  }

  printf("Recv - %s\n", buf);

  close(fd);

  return 0;
}