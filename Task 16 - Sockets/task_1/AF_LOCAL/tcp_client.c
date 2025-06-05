#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {

  struct sockaddr_un serv;

  int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (fd == -1) {
    fprintf(stderr, "Error - socket\n");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strcpy(serv.sun_path, "my_stream");

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

  close(fd);

  return 0;
}