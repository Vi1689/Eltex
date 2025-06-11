#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (fd == -1) {
    perror("Error - socket");
    return 1;
  }

  printf("Сокет создан\n");

  while (1) {
    char buf[1024] = {'\000'};
    struct sockaddr_in client;
    socklen_t len = 0;
    if ((recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len)) ==
        -1) {
      perror("Error - recv\n");
      return 1;
    }

    struct iphdr *ip_header = (struct iphdr *)buf;

    char src[16], dst[16];

    inet_ntop(AF_INET, &(ip_header->saddr), src, 16);
    inet_ntop(AF_INET, &(ip_header->daddr), dst, 16);

    printf("Source IP: %s\n", src);
    printf("Dest IP: %s\n", dst);
  }

  return 0;
}