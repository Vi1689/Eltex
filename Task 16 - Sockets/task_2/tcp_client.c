#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  struct sockaddr_in serv;

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Error - socket");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(40010);
  if (inet_aton("127.0.0.10", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  if ((connect(fd, (struct sockaddr *)&serv, sizeof(serv))) == -1) {
    perror("Error - connect");
    return 1;
  }

  char buf[100] = {'\000'};
  strcpy(buf, "What time is it?");

  if ((send(fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - send");
    return 1;
  }

  printf("Send - %s\n", buf);

  if ((recv(fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - recv");
    return 1;
  }

  printf("Recv - %s\n", buf);

  close(fd);

  return 0;
}