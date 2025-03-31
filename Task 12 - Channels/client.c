#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int fd;
  if ((fd = open("lolkek223", O_RDONLY)) == -1) {
    fprintf(stderr, "open error\n");
    return 1;
  }

  char buf;

  while (read(fd, &buf, 1) > 0) {
    write(STDOUT_FILENO, &buf, 1);
  }

  close(fd);

  unlink("lolkek223");

  return 0;
}