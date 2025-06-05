#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
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

  fd_set rfds, active_fds;
  struct timeval tv;

  FD_ZERO(&active_fds);
  FD_SET(tcp, &active_fds);
  FD_SET(udp, &active_fds);

  int new_fd;

  while (1) {
    memcpy(&rfds, &active_fds, sizeof(rfds));
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int ready = select(1024, &rfds, NULL, NULL, &tv);
    if (ready < 0) {
      perror("Error - select\n");
      return 1;
    }
    if (ready == 0) {
      continue;
    }

    for (int fd = 0; fd < 1024; ++fd) {
      if (FD_ISSET(fd, &rfds) && fd == tcp) {
        socklen_t len = sizeof(client_tcp);
        if ((new_fd = accept(tcp, (struct sockaddr *)&client_tcp, &len)) < 0)
          continue;

        FD_SET(new_fd, &active_fds);
      } else if (FD_ISSET(fd, &rfds) && fd == udp) {

        char buf[100] = {'\000'};

        socklen_t len = sizeof(client_udp);
        if ((recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client_udp,
                      &len)) == -1) {
          perror("Error - recv\n");
          return 1;
        }

        printf("Recv - %s\n", buf);

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

        if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client_udp,
                    sizeof(client_udp))) == -1) {
          perror("Error - send\n");
          return 1;
        }

        printf("Send - %s\n", buf);

      } else if (FD_ISSET(fd, &rfds)) {
        char buf[100] = {'\000'};

        if ((recv(fd, buf, sizeof(buf), 0)) == -1) {
          perror("Error - recv");
          return 1;
        }

        printf("Recv - %s\n", buf);

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);

        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

        if ((send(fd, buf, sizeof(buf), 0)) == -1) {
          perror("Error - send");
          return 1;
        }

        printf("Send - %s\n", buf);

        FD_CLR(fd, &active_fds);
        close(fd);
      }
    }
  }

  close(tcp);
  close(udp);
  return 0;
}