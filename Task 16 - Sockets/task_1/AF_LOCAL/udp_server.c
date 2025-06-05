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

  char path[] = "my_stream";

  unlink(path);

  memset(&serv, 0, sizeof(serv));
  serv.sun_family = AF_LOCAL;
  strcpy(serv.sun_path, path);

  if ((bind(fd, (struct sockaddr *)&serv, sizeof(serv))) == -1) {
    fprintf(stderr, "Error - bind\n");
    return 1;
  }

  char buf[100] = {'\000'};

  socklen_t len = sizeof(client);
  if ((recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len)) ==
      -1) {
    fprintf(stderr, "Error - recv\n");
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

  if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
              sizeof(client))) == -1) {
    fprintf(stderr, "Error - send\n");
    return 1;
  }

  printf("Send - %s\n", buf);

  close(fd);

  unlink(path);

  return 0;
}