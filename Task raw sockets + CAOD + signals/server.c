#include "my_list.h"
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <malloc.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8888

int end = 1;

void my_recv(int fd, char *buf, size_t size_buf) {
  struct sockaddr_in client;
  socklen_t len = sizeof(client);
  if ((recvfrom(fd, buf, size_buf, 0, (struct sockaddr *)&client, &len)) ==
      -1) {
    perror("Error - recv");
    exit(1);
  }
}

void my_send(int fd, uint16_t port, uint32_t ip, char *data, size_t count) {

  char str_conut[10] = {'\000'};
  sprintf(str_conut, "%ld", count);

  strcat(data, " ");
  strcat(data, str_conut);

  struct sockaddr_in client = {0};

  client.sin_family = AF_INET;
  client.sin_port = port;
  client.sin_addr.s_addr = ip;

  struct udphdr udp_hdr = {0};

  udp_hdr.uh_dport = port;
  udp_hdr.uh_sport = htons(PORT);
  udp_hdr.uh_ulen = htons(16);

  struct iphdr ip_hdr = {0};

  ip_hdr.version = 4;
  ip_hdr.ihl = 5;
  ip_hdr.protocol = IPPROTO_UDP;
  ip_hdr.daddr = ip;

  char buf[1024] = {'\000'};

  memcpy(buf, &ip_hdr, sizeof(ip_hdr));

  memcpy(buf + sizeof(ip_hdr), &udp_hdr, sizeof(udp_hdr));

  strcat(buf + sizeof(udp_hdr) + sizeof(ip_hdr), data);

  if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
              sizeof(client))) == -1) {
    perror("Error - send");
    exit(1);
  }
}

void fun() { end = 0; }

int main() {

  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGINT);

  struct sigaction act = {0};

  act.sa_mask = set;
  act.sa_flags = SA_RESTART;
  act.sa_sigaction = &fun;
  if (sigaction(SIGINT, &act, NULL) == -1) {
    perror("Error - sigaction");
    exit(1);
  }

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

  struct list *client = create();

  while (end) {
    char buf[1024] = {'\000'};
    my_recv(fd, buf, sizeof(buf));

    struct udphdr *udp_hdr = (struct udphdr *)(buf + sizeof(struct iphdr));

    struct iphdr *ip_hdr = (struct iphdr *)(buf);

    if (ntohs(udp_hdr->uh_dport) == PORT && ip_hdr->protocol == IPPROTO_UDP) {

      uint16_t port = udp_hdr->source;
      uint32_t ip = ip_hdr->saddr;

      char *data = buf + sizeof(struct iphdr) + sizeof(struct udphdr);

      if (strcmp(data, "exit") == 0) {
        pop(client, ip, port);
        continue;
      }

      my_send(fd, port, ip, data, push(client, ip, port));
    }
  }

  close(fd);
  delete (client);

  exit(0);
}