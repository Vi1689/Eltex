#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 1) {
    printf("Введите pid\n");
    return 0;
  }
  kill(strtol(argv[1], NULL, 10), SIGUSR1);
  return 0;
}