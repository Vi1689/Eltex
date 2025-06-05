#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {

  struct sockaddr_un serv, client;

  int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
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

  if ((listen(fd, 5)) == -1) {
    fprintf(stderr, "Error - listen\n");
    return 1;
  }

  socklen_t len = sizeof(client);
  int new_fd = accept(fd, (struct sockaddr *)&client, &len);

  if (new_fd == -1) {
    fprintf(stderr, "Error - accept\n");
    return 1;
  }

  char buf[100] = {'\000'};

  if ((recv(new_fd, buf, sizeof(buf), 0)) == -1) {
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

  if ((send(new_fd, buf, sizeof(buf), 0)) == -1) {
    fprintf(stderr, "Error - send\n");
    return 1;
  }

  printf("Send - %s\n", buf);

  close(fd);

  close(new_fd);

  unlink(path);

  return 0;
}