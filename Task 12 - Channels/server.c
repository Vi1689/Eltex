#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  if (mkfifo("lolkek223", S_IRWXU) == -1) {
    fprintf(stderr, "mkfifo error\n");
    return 1;
  }

  int fd;
  if ((fd = open("lolkek223", O_WRONLY)) == -1) {
    fprintf(stderr, "open error\n");
    return 1;
  }

  write(fd, "Hi!\n", strlen("Hi!\n"));

  close(fd);
  return 0;
}