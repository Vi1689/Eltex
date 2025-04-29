#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

int main() {
  printf("Pid = %d\n", getpid());

  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGINT);

  if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  while (1) {
  }
  return 0;
}