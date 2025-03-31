#include <errno.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void clear_memory(char ***arg, size_t *number_arg, size_t prog) {
  for (size_t i = 0; i < prog; ++i) {
    for (size_t j = 0; j < number_arg[i]; ++j) {
      free(arg[i][j]);
    }
    free(arg[i]);
  }
  free(arg);
  free(number_arg);
}

int main() {

  pid_t pid;
  int stop = 1;

  while (stop) {
    char ***arg = malloc(sizeof(char **) * 1);
    arg[0] = malloc(sizeof(char *) * 1);
    arg[0][0] = malloc(sizeof(char) * 128);
    memset(arg[0][0], '\000', 128);

    size_t *number_arg = malloc(sizeof(size_t) * 1);
    number_arg[0] = 0;

    printf("Введите имя программы и её опции или exit для выхода\n");

    int charc, flag = 0;
    size_t prog = 0, count = 0, count_number = 0;
    while ((charc = fgetc(stdin)) != '\n') {
      if (charc == ' ') {
        if (!flag) {
          flag = 1;
        }
        continue;
      } else {
        if (flag) {
          flag = 0;
          if (arg[prog][0][0] != '\000' && charc != '|') {
            count = 0;
            number_arg[prog]++;
            arg[prog] =
                realloc(arg[prog], (number_arg[prog] + 1) * sizeof(char *));
            arg[prog][number_arg[prog]] = malloc(sizeof(char) * 128);
            memset(arg[prog][number_arg[prog]], '\000', 128);
          }
        }
      }

      if (charc == '|') {
        count_number++;
        prog++;
        arg = realloc(arg, (prog + 1) * sizeof(char **));
        number_arg = realloc(number_arg, (count_number + 1) * sizeof(size_t));
        number_arg[prog] = 0;
        count = 0;
        arg[prog] = malloc(sizeof(char *) * 1);
        arg[prog][number_arg[prog]] = malloc(sizeof(char) * 128);
        memset(arg[prog][number_arg[prog]], '\000', 128);
        continue;
      }
      arg[prog][number_arg[prog]][count++] = charc;
    }

    prog++;
    for (size_t i = 0; i < prog; ++i) {
      number_arg[i]++;
      arg[i] = realloc(arg[i], (number_arg[i] + 1) * sizeof(char *));
      arg[i][number_arg[i]] = NULL;
    }

    if (strcmp(arg[0][0], "exit") == 0) {
      clear_memory(arg, number_arg, prog);
      break;
    }

    int fd[2], prev_pipe_read = -1;

    for (size_t i = 0; i < prog; ++i) {
      pipe(fd);
      switch (pid = fork()) {
      case -1:
        perror("Процесс не создан\n");
        clear_memory(arg, number_arg, prog);
        exit(1);
      case 0:

        if (prog > 1) {
          if (i > 0) {
            dup2(prev_pipe_read, STDIN_FILENO);
            close(prev_pipe_read);
          }

          if (i != (prog - 1)) {
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            close(fd[0]);
          }
        }

        if (execv(arg[i][0], arg[i]) == -1) {
          perror("Программа не запущена\n");
          clear_memory(arg, number_arg, prog);
          exit(1);
        }
      default:
        if (i > 0) {
          close(prev_pipe_read);
        }
        if (i != (prog - 1)) {
          close(fd[1]);
          prev_pipe_read = fd[0];
        }
        break;
      }
    }
    while (wait(NULL) > 0) {
    }
    clear_memory(arg, number_arg, prog);
  }
  exit(0);
}