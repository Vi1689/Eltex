#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main() {
  int tcp, udp;

  struct sockaddr_in serv_tcp, serv_udp, client_tcp, client_udp;

  tcp = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp == -1) {
    perror("Error - socket");
    return 1;
  }
  udp = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp == -1) {
    perror("Error - socket");
    return 1;
  }

  memset(&serv_tcp, 0, sizeof(serv_tcp));

  serv_tcp.sin_family = AF_INET;
  serv_tcp.sin_port = htons(40010);
  if (inet_aton("127.0.0.10", &serv_tcp.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  if (bind(tcp, (struct sockaddr *)&serv_tcp, sizeof(serv_tcp)) == -1) {
    perror("Error - bind");
    return 1;
  }

  if ((listen(tcp, 5)) == -1) {
    perror("Error - listen");
    return 1;
  }

  memset(&serv_udp, 0, sizeof(serv_udp));

  serv_udp.sin_family = AF_INET;
  serv_udp.sin_port = htons(40011);
  if (inet_aton("127.0.0.11", &serv_udp.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  if ((bind(udp, (struct sockaddr *)&serv_udp, sizeof(serv_udp))) == -1) {
    perror("Error - bind\n");
    return 1;
  }

  struct pollfd *pfds;

  pfds = calloc(100, sizeof(struct pollfd));
  if (pfds == NULL) {
    perror("Error - calloc");
    return 1;
  }

  int count = 2;

  for (int i = 0; i < 100; ++i) {
    pfds[i].events = POLLERR;
  }

  pfds[0].fd = tcp;
  pfds[0].events = POLLIN;
  pfds[1].fd = udp;
  pfds[1].events = POLLIN;

  int new_fd;

  while (1) {
    int ready = poll(pfds, 100, -1);
    if (ready < 0) {
      perror("Error - poll\n");
      return 1;
    }

    for (int i = 0; i < 100; ++i) {
      if (pfds[i].revents != 0 && pfds[i].fd == tcp) {
        socklen_t len = sizeof(client_tcp);
        if ((new_fd = accept(tcp, (struct sockaddr *)&client_tcp, &len)) < 0)
          continue;
        pfds[count].fd = new_fd;
        pfds[count++].events = POLLIN;
      } else if (pfds[i].revents != 0 && pfds[i].fd == udp) {
        char buf[100] = {'\000'};

        socklen_t len = sizeof(client_udp);
        if ((recvfrom(pfds[i].fd, buf, sizeof(buf), 0,
                      (struct sockaddr *)&client_udp, &len)) == -1) {
          perror("Error - recv\n");
          return 1;
        }

        printf("Recv - %s\n", buf);

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

        if ((sendto(pfds[i].fd, buf, sizeof(buf), 0,
                    (struct sockaddr *)&client_udp, sizeof(client_udp))) ==
            -1) {
          perror("Error - send\n");
          return 1;
        }

        printf("Send - %s\n", buf);
      } else if (pfds[i].revents != 0 && (pfds[i].revents & POLLIN)) {
        char buf[100] = {'\000'};

        if ((recv(pfds[i].fd, buf, sizeof(buf), 0)) == -1) {
          perror("Error - recv");
          return 1;
        }

        printf("Recv - %s\n", buf);

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

        if ((send(pfds[i].fd, buf, sizeof(buf), 0)) == -1) {
          perror("Error - send");
          return 1;
        }

        printf("Send - %s\n", buf);

        close(pfds[i].fd);
        pfds[i].fd = 0;
      }
    }
  }
  close(tcp);
  close(udp);
  return 0;
}