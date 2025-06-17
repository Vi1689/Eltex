#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_PORT 8888

void my_recv(int fd, char *buf, size_t size_buf) {
  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  if ((recvfrom(fd, buf, size_buf, 0, (struct sockaddr *)&client, &len)) ==
      -1) {
    perror("Error - recv");
    exit(1);
  }
}

void my_send(int fd, char *data) {

  struct sockaddr_in serv = {0};

  serv.sin_family = AF_INET;
  serv.sin_port = SERV_PORT;
  if (inet_aton("172.18.0.1", (struct in_addr *)&serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    exit(1);
  }

  struct udphdr udp_hdr = {0};

  udp_hdr.uh_dport = htons(SERV_PORT);
  udp_hdr.uh_sport = htons(9999);
  udp_hdr.uh_ulen = htons(16);

  struct iphdr ip_hdr = {0};

  ip_hdr.version = 4;
  ip_hdr.ihl = 5;
  ip_hdr.protocol = IPPROTO_UDP;
  if (inet_aton("172.18.0.1", (struct in_addr *)&ip_hdr.daddr) == 0) {
    perror("Error - inet_aton");
    exit(1);
  }

  char buf[1024] = {'\000'};

  memcpy(buf, &ip_hdr, sizeof(ip_hdr));

  memcpy(buf + sizeof(ip_hdr), &udp_hdr, sizeof(udp_hdr));

  strcat(buf + sizeof(udp_hdr) + sizeof(ip_hdr), data);

  if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&serv,
              sizeof(serv))) == -1) {
    perror("Error - send");
    exit(1);
  }
}

int main() {

  struct sockaddr_in serv = {0};

  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (fd == -1) {
    perror("Error - socket");
    exit(1);
  }

  int flag = 1;
  if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag)) == -1) {
    perror("Error - setsockopt");
    exit(1);
  }

  serv.sin_family = AF_INET;
  serv.sin_port = htons(SERV_PORT);
  if (inet_aton("172.18.0.1", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    exit(1);
  }

  char input[200];

  while (1) {

    if (fgets(input, sizeof(input), stdin) != NULL) {
      input[strcspn(input, "\n")] = '\000';
    } else {
      perror("Error - fgets");
      close(fd);
      exit(1);
    }

    if (strcmp(input, "exit") == 0) {
      my_send(fd, input);
      break;
    }

    my_send(fd, input);

    char buf[1024] = {'\000'};
    my_recv(fd, buf, sizeof(buf));

    struct udphdr *udp_hdr = (struct udphdr *)(buf + sizeof(struct iphdr));

    if (ntohs(udp_hdr->uh_dport) == 9999 &&
        ntohs(udp_hdr->uh_sport) == SERV_PORT) {
      char *data = buf + sizeof(struct udphdr) + sizeof(struct iphdr);
      printf("Recv: %s\n", data);
    }
  }

  close(fd);

  exit(0);
}