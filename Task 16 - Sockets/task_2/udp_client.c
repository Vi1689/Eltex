#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {

  struct sockaddr_in serv, client;

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    perror("Error - socket\n");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(40011);
  if (inet_aton("127.0.0.11", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  memset(&client, 0, sizeof(client));
  client.sin_family = AF_INET;
  client.sin_port = htons(40002);
  if (inet_aton("127.0.0.2", &client.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  if ((bind(fd, (struct sockaddr *)&client, sizeof(client))) == -1) {
    perror("Error - bind\n");
    return 1;
  }

  if ((connect(fd, (struct sockaddr *)&serv, sizeof(serv))) == -1) {
    perror("Error - connect\n");
    return 1;
  }

  char buf[100] = {'\000'};
  strcpy(buf, "What time is it?");

  if ((send(fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - send\n");
    return 1;
  }

  printf("Send - %s\n", buf);

  if ((recv(fd, buf, sizeof(buf), 0)) == -1) {
    perror("Error - recv\n");
    return 1;
  }

  printf("Recv - %s\n", buf);

  close(fd);

  return 0;
}