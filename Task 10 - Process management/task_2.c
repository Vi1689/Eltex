#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid[5];

  switch (pid[0] = fork()) {
  case -1:
    perror("Процесс не создан\n");
    exit(1);
  case 0:
    printf("Это дочерний процесс 1\npid = %d\nppid = %d\n", getpid(),
           getppid());
    switch (pid[2] = fork()) {
    case -1:
      perror("Процесс не создан\n");
      exit(1);
    case 0:
      printf("Это дочерний процесс 3\npid = %d\nppid = %d\n", getpid(),
             getppid());
      break;
    default:
      switch (pid[3] = fork()) {
      case -1:
        perror("Процесс не создан\n");
        exit(1);
      case 0:
        printf("Это дочерний процесс 4\npid = %d\nppid = %d\n", getpid(),
               getppid());
        break;
      default:
        wait(NULL);
        break;
      }
      wait(NULL);
      break;
    }
    break;
  default:
    printf("Это родительский процесс\npid = %d\nppid = %d\n", getpid(),
           getppid());
    switch (pid[1] = fork()) {
    case -1:
      perror("Процесс не создан\n");
      exit(1);
    case 0:
      printf("Это дочерний процесс 2\npid = %d\nppid = %d\n", getpid(),
             getppid());
      switch (pid[4] = fork()) {
      case -1:
        perror("Процесс не создан\n");
        exit(1);
      case 0:
        printf("Это дочерний процесс 5\npid = %d\nppid = %d\n", getpid(),
               getppid());
        break;
      default:
        wait(NULL);
        break;
      }
      break;
    default:
      wait(NULL);
      break;
    }
    wait(NULL);
    break;
  }
  exit(0);
}