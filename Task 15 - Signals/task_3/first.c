#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

int main() {
  printf("Pid = %d\n", getpid());

  int sig_num;

  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  while (1) {
    sigwait(&set, &sig_num);
    printf("Sig number %d\n", sig_num);
  }
  return 0;
}