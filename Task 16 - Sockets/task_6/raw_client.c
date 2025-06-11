#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  struct sockaddr_ll serv;

  int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (fd == -1) {
    perror("Error - socket\n");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));

  serv.sll_family = AF_PACKET;
  serv.sll_halen = 6;
  serv.sll_ifindex = if_nametoindex("eth0");
  unsigned char dest_mac[6] = {0x02, 0x42, 0x40, 0x8f, 0x9a, 0xd2};
  memcpy(serv.sll_addr, dest_mac, 6);

  struct udphdr udp_hdr = {0};

  udp_hdr.uh_dport = htons(40000);
  udp_hdr.uh_sport = htons(9999);
  udp_hdr.uh_ulen = htons(sizeof(udp_hdr) + strlen("Hello") + 1);
  udp_hdr.uh_sum = 0;

  struct iphdr ip_hdr = {0};

  ip_hdr.version = 4;
  ip_hdr.ihl = 5;
  ip_hdr.ttl = 64;
  ip_hdr.tot_len =
      htons(sizeof(ip_hdr) + sizeof(udp_hdr) + strlen("Hello") + 1);
  ip_hdr.id = htons(54321);
  ip_hdr.protocol = IPPROTO_UDP;
  if (inet_aton("172.18.0.2", (struct in_addr *)&ip_hdr.saddr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }
  ip_hdr.id = 0;
  if (inet_aton("172.18.0.1", (struct in_addr *)&ip_hdr.daddr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  int csum = 0;
  short *ptr = (short *)&ip_hdr;

  for (int i = 0; i < 10; ++i) {
    csum += *ptr;
    ptr++;
  }
  int tmp = csum >> 16;
  csum = (csum & 0xFFFF) + tmp;
  csum = ~csum;

  ip_hdr.check = (csum & 0xFFFF);

  struct ethhdr eth_hrd = {0};

  memcpy(eth_hrd.h_dest, dest_mac, 6);

  unsigned char src_mac[6] = {0x02, 0x42, 0xAC, 0x12, 0x00, 0x02};

  memcpy(eth_hrd.h_source, src_mac, 6);

  eth_hrd.h_proto = htons(ETH_P_IP);

  char buf[128] = {'\000'};

  memcpy(buf, &eth_hrd, sizeof(eth_hrd));

  memcpy(buf + sizeof(eth_hrd), &ip_hdr, sizeof(ip_hdr));

  memcpy(buf + sizeof(eth_hrd) + sizeof(ip_hdr), &udp_hdr, sizeof(udp_hdr));

  strcat(buf + sizeof(eth_hrd) + sizeof(udp_hdr) + sizeof(ip_hdr), "Hello");

  if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&serv,
              sizeof(serv))) == -1) {
    perror("Error - send\n");
    return 1;
  }

  printf("Send - %s\n",
         buf + sizeof(eth_hrd) + sizeof(udp_hdr) + sizeof(ip_hdr));

  while (1) {
    char buf[1024] = {'\000'};
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    if ((recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len)) ==
        -1) {
      perror("Error - recv\n");
      return 1;
    }

    struct udphdr *udp_hdr =
        (struct udphdr *)(buf + sizeof(eth_hrd) + sizeof(ip_hdr));

    if (ntohs(udp_hdr->uh_dport) == 9999 && ntohs(udp_hdr->uh_sport) == 40000) {
      char *data = buf + sizeof(eth_hrd) + sizeof(ip_hdr) + sizeof(udp_hdr);
      printf("Recv: %s\n", data);
      break;
    }
  }

  close(fd);

  return 0;
}