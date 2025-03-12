#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void clear_memory(char **arg, size_t size) {
  for (size_t i = 0; i <= size; ++i) {
    free(arg[i]);
  }
  free(arg);
}

int main() {

  pid_t pid;
  int stop = 1;

  while (stop) {
    char **arg = malloc(sizeof(char *) * 1);
    arg[0] = malloc(sizeof(char) * 128);
    memset(arg[0], '\0', 128);
    printf("Введите имя программы и её опции или exit для выхода\n");

    int c, count = 0;
    size_t size = 0;
    while ((c = fgetc(stdin)) != '\n') {
      if (c == ' ') {
        size++;
        arg = realloc(arg, (size + 1) * sizeof(char *));
        arg[size] = malloc(sizeof(char) * 128);
        memset(arg[size], '\0', 128);
        count = 0;
        continue;
      }
      arg[size][count++] = c;
    }

    if (strcmp(arg[0], "exit") == 0) {
      clear_memory(arg, size);
      break;
    }

    switch (pid = fork()) {
    case -1:
      perror("Процесс не создан\n");
      exit(1);
    case 0:
      if (execv(arg[0], arg) == -1) {
        perror("Программа не запущена\n");
        exit(1);
      }
      exit(0);
      break;
    default:
      wait(NULL);
      break;
    }

    clear_memory(arg, size);
  }
  exit(0);
}