#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int pipefd[2];

  if (pipe(pipefd) == -1) {
    fprintf(stderr, "Pipe error\n");
    return 1;
  }

  pid_t cpid = fork();
  if (cpid == -1) {
    fprintf(stderr, "Fork error\n");
    return 1;
  }

  char buf;

  if (cpid == 0) {
    close(pipefd[1]);
    // чтение | поток ввода
    while (read(pipefd[0], &buf, 1) > 0) {
      write(STDOUT_FILENO, &buf, 1);
    }

    write(STDOUT_FILENO, "\n", 1);
    close(pipefd[0]);
    return 0;

  } else {
    close(pipefd[0]);
    // запись | поток вывода
    write(pipefd[1], "Hi!", strlen("Hi!"));
    close(pipefd[1]);
    wait(NULL);
    return 0;
  }
}