#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;
  int rv;

  switch (pid = fork()) {
  case -1:
    perror("Процесс не создан\n");
    exit(1);
  case 0:
    printf("Это дочерний процесс\npid = %d\nppid = %d\n", getpid(), getppid());
    break;
  default:
    printf("Это родительский процесс\npid = %d\nppid = %d\n", getpid(),
           getppid());
    wait(&rv);
    printf("Статус завершения дочернего процесса - %d\n", WEXITSTATUS(rv));
    break;
  }
  exit(0);
}