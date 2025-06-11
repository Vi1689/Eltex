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

  struct sockaddr_in serv;

  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (fd == -1) {
    perror("Error - socket\n");
    return 1;
  }

  int flag = 1;
  if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) == -1) {
    perror("Error - setsockopt");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(40000);
  if (inet_aton("127.0.0.1", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  struct udphdr udp_hdr = {0};

  udp_hdr.dest = htons(40000);
  udp_hdr.source = htons(9999);
  udp_hdr.len = htons(16);
  udp_hdr.check = 0;

  struct iphdr ip_hdr = {0};

  ip_hdr.version = 4;
  ip_hdr.ihl = 5;
  ip_hdr.protocol = IPPROTO_UDP;
  ip_hdr.saddr = 0;
  ip_hdr.id = 0;
  if (inet_aton("127.0.0.1", (struct in_addr *)&ip_hdr.daddr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  char buf[100] = {'\000'};

  memcpy(buf, &ip_hdr, sizeof(ip_hdr));

  memcpy(buf + sizeof(ip_hdr), &udp_hdr, sizeof(udp_hdr));

  strcat(buf + sizeof(udp_hdr) + sizeof(ip_hdr), "Hello");

  if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&serv,
              sizeof(serv))) == -1) {
    perror("Error - send\n");
    return 1;
  }

  printf("Send - %s\n", buf + sizeof(udp_hdr) + sizeof(ip_hdr));

  while (1) {
    char buf[1024] = {'\000'};
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    if ((recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len)) ==
        -1) {
      perror("Error - recv\n");
      return 1;
    }

    struct udphdr *udp_hdr = (struct udphdr *)(buf + 20);

    if (ntohs(udp_hdr->dest) == 9999 && ntohs(udp_hdr->source) == 40000) {
      char *data = buf + 20 + 8;
      printf("Recv: %s\n", data);
      break;
    }
  }

  close(fd);

  return 0;
}