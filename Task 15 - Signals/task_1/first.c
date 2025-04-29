#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void fun(int signo, siginfo_t *info, void *context) {
  int *arg = (int *)context;
  printf("Сигнал получен %d %d %d\n", signo, *arg, info->si_signo);
}

int main() {
  printf("Pid = %d\n", getpid());

  sigset_t set;

  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);

  struct sigaction act = {0};

  act.sa_mask = set;
  act.sa_flags = SA_RESTART;
  act.sa_sigaction = &fun;
  if (sigaction(SIGUSR1, &act, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  while (1) {
  }
  return 0;
}