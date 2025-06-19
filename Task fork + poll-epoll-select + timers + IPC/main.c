#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct driver {
  char task;
  pid_t pid;
  int task_timer;
};

struct driver_info {
  char task;
  pid_t pid;
  int task_timer;
  int fd;
};

void fun_driver(int root, int answers) {

  struct driver send = {0};

  pid_t pid = getpid();

  send.pid = pid;

  char msend[100] = {'\000'};

  memcpy(msend, &send, sizeof(send));

  int tmp = mq_send(root, msend, 100, send.pid);
  if (tmp == -1) {
    perror("mq_send");
    exit(1);
  }

  sleep(3);

  char qname[32] = {'\000'};
  snprintf(qname, sizeof(qname), "/pid_%d", pid);
  mqd_t qeuq = mq_open(qname, O_RDWR);
  if (qeuq == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }

  int end = 1;

  while (end) {
    char msend[100] = {'\000'};
    char mrecv[100] = {'\000'};

    int tmp = mq_receive(qeuq, mrecv, 100, NULL);
    if (tmp == -1) {
      perror("mq_receive driver");
      exit(1);
    }

    struct driver *recv = (struct driver *)mrecv;

    if (recv->task == 'w') {

      sleep(recv->task_timer);

      struct driver send = {0};

      send.pid = pid;
      send.task = 'a';
      send.task_timer = 0;

      memcpy(msend, &send, sizeof(send));

      tmp = mq_send(answers, msend, 100, 1);
      if (tmp == -1) {
        perror("mq_send");
        exit(1);
      }
    } else if (recv->task == 'e') {
      end = 0;
    }
  }
}

int main() {

  char name_root[] = "/root";
  char name_answers[] = "/answers";

  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10; // Максимальное количество сообщений
  attr.mq_msgsize = 100; // Максимальный размер сообщения
  attr.mq_curmsgs = 0;

  mqd_t root = mq_open(name_root, O_RDWR | O_CREAT, S_IRWXU, &attr);
  if (root == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }

  mqd_t answers = mq_open(name_answers, O_RDWR | O_CREAT, S_IRWXU, &attr);
  if (answers == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }

  fd_set rfds, active_fds;
  struct timeval tv;

  FD_ZERO(&active_fds);
  FD_SET(STDIN_FILENO, &active_fds);
  FD_SET(root, &active_fds);
  FD_SET(answers, &active_fds);

  char input;

  printf("1 - create_driver\n2 - send_task <pid> <task_timer>\n3 - "
         "get_status "
         "<pid>\n4 - get_drivers\n5 - exit\n");

  struct driver_info *fd_driver = NULL;
  int count_driver = 0;

  int end = 1;

  while (end) {
    memcpy(&rfds, &active_fds, sizeof(rfds));
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    int ready = select(1024, &rfds, NULL, NULL, &tv);
    if (ready < 0) {
      perror("Error - select");
      return 1;
    }
    if (ready == 0) {
      continue;
    }

    for (int fd = 0; fd < 1024; ++fd) {
      if (FD_ISSET(fd, &rfds) && fd == STDIN_FILENO) {

        pid_t pid = 0;

        scanf("%c", &input);

        switch (input) {
        case '1':

          pid = fork();
          if (pid == -1) {
            perror("Процесс не создан\n");
            exit(1);
          } else if (!pid) {
            fun_driver(root, answers);

            break;
          } else {
            char mrecv[100] = {'\000'};
            int tmp = mq_receive(root, mrecv, 100, NULL);
            if (tmp == -1) {
              perror("msgrcv");
              exit(1);
            }

            struct driver *recv = (struct driver *)mrecv;

            char qname[32] = {'\000'};
            snprintf(qname, sizeof(qname), "/pid_%d", recv->pid);

            fd_driver = realloc(fd_driver, (count_driver + 1) *
                                               sizeof(struct driver_info));

            fd_driver[count_driver].fd =
                mq_open(qname, O_RDWR | O_CREAT, S_IRWXU, &attr);
            if (fd_driver[count_driver].fd == (mqd_t)-1) {
              perror("mq_open");
              exit(1);
            }

            FD_SET(fd_driver[count_driver].fd, &active_fds);

            fd_driver[count_driver].pid = recv->pid;
            fd_driver[count_driver].task = 'a';
            fd_driver[count_driver].task_timer = 0;
          }

          printf("Driver create\n");

          count_driver++;

          break;

        case '2':

          pid_t pid2;
          int time;

          printf("Введите pid и time - ");
          scanf("%d %d", &pid2, &time);

          printf("Task add <%d> <%d>\n", pid2, time);

          struct driver send = {0};

          send.pid = pid2;
          send.task = 'w';
          send.task_timer = time;

          char msend[100] = {'\000'};

          int found2 = -1;
          for (int i = 0; i < count_driver; ++i) {
            if (fd_driver[i].pid == pid2) {
              if (fd_driver[i].task == 'w') {
                found2 = i;
                break;
              }
              fd_driver[i].task = 'w';
              fd_driver[i].task_timer = time;

              memcpy(msend, &send, sizeof(send));

              int tmp = mq_send(fd_driver[i].fd, msend, 100, 1);
              if (tmp == -1) {
                perror("mq_send");
                exit(1);
              }
              found2 = -2;
              break;
            }
          }

          if (found2 == -1) {
            printf("Driver with pid %d not found\n", pid2);
          } else if (found2 != -2) {
            printf("Busy <%d>\n", fd_driver[found2].task_timer);
          }

          break;

        case '3':

          pid_t pid3;

          printf("Введите pid - ");
          scanf("%d", &pid3);

          printf("Status <%d>\n", pid3);

          int found3 = 0;
          for (int i = 0; i < count_driver; ++i) {
            if (fd_driver[i].pid == pid3) {
              printf("Pid - %d\n", fd_driver[i].pid);
              if (fd_driver[i].task == 'a') {
                printf("Available\n");
              } else {
                printf("Busy %d\n", fd_driver[i].task_timer);
              }
              found3 = 1;
              break;
            }
          }

          if (!found3) {
            printf("Driver with pid %d not found\n", pid3);
          }

          break;

        case '4':
          printf("get_drivers\n");
          for (int i = 0; i < count_driver; ++i) {
            printf("Pid - %d\n", fd_driver[i].pid);
            if (fd_driver[i].task == 'a') {
              printf("Available\n");
            } else {
              printf("Busy %d\n", fd_driver[i].task_timer);
            }
          }

          break;

        case '5':

          struct driver quit = {0};

          for (int i = 0; i < count_driver; ++i) {

            quit.pid = fd_driver[i].pid;
            quit.task = 'e';

            char mquit[100] = {'\000'};
            memcpy(mquit, &quit, sizeof(exit));

            int tmp = mq_send(fd_driver[i].fd, mquit, 100, 1);
            if (tmp == -1) {
              perror("mq_send");
              exit(1);
            }
          }

          end = 0;

          break;

        default:

          break;
        }

      } else if (FD_ISSET(fd, &rfds)) {

        char mrecv[100] = {'\000'};
        int tmp = mq_receive(answers, mrecv, 100, NULL);
        if (tmp == -1) {
          perror("msgrcv");
          exit(1);
        }

        struct driver *recv = (struct driver *)mrecv;

        for (int i = 0; i < count_driver; ++i) {
          if (fd_driver[i].pid == recv->pid) {
            fd_driver[i].task = recv->task;
            fd_driver[i].task_timer = recv->task_timer;
          }
        }
      }
    }
  }

  for (int i = 0; i < count_driver; ++i) {
    mq_close(fd_driver[i].fd);
    char qname[32] = {'\000'};
    snprintf(qname, sizeof(qname), "/pid_%d", fd_driver[i].pid);
    mq_unlink(qname);
  }

  free(fd_driver);

  mq_close(root);
  mq_unlink(name_root);

  mq_close(answers);
  mq_unlink(name_answers);

  exit(0);
}