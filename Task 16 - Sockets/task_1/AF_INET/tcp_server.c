#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  struct sockaddr_in serv, client;

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Error - socket");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));

  serv.sin_family = AF_INET;
  serv.sin_port = htons(40000);
  if (inet_aton("127.0.0.1", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  printf("Port - %d\nIP - %s\n", htons(serv.sin_port),
         inet_ntoa(serv.sin_addr));

  if (bind(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
    perror("Error - bind");
    return 1;
  }

  if ((listen(fd, 5)) == -1) {
    perror("Error - listen");
    return 1;
  }

  socklen_t len = sizeof(client);
  int new_fd = accept(fd, (struct sockaddr *)&client, &len);

  if (new_fd == -1) {
    perror("Error - accept");
    return 1;
  }

  char buf[100] = {'\000'};

  if ((recv(new_fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - recv");
    return 1;
  }

  printf("Recv - %s\n", buf);

  for (int i = 0; buf[i] != '\000'; ++i) {
    if (buf[i] >= 'A' && buf[i] <= 'Z') {
      buf[i] += 'a' - 'A';
    } else {
      buf[i] -= 'a' - 'A';
    }
  }

  if ((send(new_fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - send");
    return 1;
  }

  printf("Send - %s\n", buf);

  close(fd);

  close(new_fd);

  return 0;
}