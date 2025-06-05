#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {

  struct sockaddr_un serv, client;

  int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (fd == -1) {
    fprintf(stderr, "Error - socket\n");
    return 1;
  }

  char path[] = "my_clinet";

  unlink(path);

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strcpy(serv.sun_path, "my_stream");

  memset(&client, 0, sizeof(client));
  client.sun_family = AF_LOCAL;
  strcpy(client.sun_path, path);

  if ((bind(fd, (struct sockaddr *)&client, sizeof(client))) == -1) {
    fprintf(stderr, "Error - bind\n");
    return 1;
  }

  if ((connect(fd, (struct sockaddr *)&serv, sizeof(serv))) == -1) {
    fprintf(stderr, "Error - connect\n");
    return 1;
  }

  char buf[100] = {'\000'};
  strcpy(buf, "Hello");

  if ((send(fd, buf, sizeof(buf), 0)) == -1) {
    fprintf(stderr, "Error - send\n");
    return 1;
  }

  printf("Send - %s\n", buf);

  if ((recv(fd, buf, sizeof(buf), 0)) == -1) {
    fprintf(stderr, "Error - recv\n");
    return 1;
  }

  printf("Recv - %s\n", buf);

  unlink(path);

  close(fd);

  return 0;
}