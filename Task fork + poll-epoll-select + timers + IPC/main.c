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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct msgbuf {
  long mtype;
  pid_t pid;
  int task_timer;
  // char mtext[1024];
};

void driver(struct msgbuf *msg, int root, int broadcast) {

  pid_t pid = getpid();

  broadcast = 100;
  broadcast++;

  // memset(msg->mtext, '\000', sizeof(msg->mtext));
  msg->pid = pid;
  msg->mtype = 1;
  ssize_t tmp = msgsnd(root, msg, sizeof(msg), 0);
  if (tmp == -1) {
    perror("msgrcv");
    exit(1);
  }

  //   while (1) {
  // memset(msg->mtext, '\000', sizeof(msg->mtext));
  // ssize_t tmp = msgrcv(root, &msg, sizeof(msg), pid, 0);
  // if (tmp == -1) {
  //   perror("msgrcv");
  //   exit(1);
  // }

  // sleep(msg->task_timer);
  //   }
}

int main() {

  key_t key_root = ftok("Makefile", 100),
        key_broadcast = ftok("Makefile", 1000);

  int root = msgget(key_root, 0666 | IPC_CREAT);
  if (root == -1) {
    perror("msgget root");
    exit(1);
  }

  int broadcast = msgget(key_broadcast, 0666 | IPC_CREAT);
  if (broadcast == -1) {
    perror("msgget broadcast");
    exit(1);
  }

  struct msgbuf msg = {0};

  char input;

  printf("1 - create_driver\n2 - send_task <pid> <task_timer>\n3 - get_status "
         "<pid>\n4 - get_drivers\n5 - exit\n");

  scanf("%c", &input);

  pid_t pid = 0;

  switch (input) {
  case '1':

    switch (pid = fork()) {
    case -1:
      perror("Процесс не создан\n");
      exit(1);

    case 0:
      printf("Это дочерний процесс\npid = %d\nppid = %d\n", getpid(),
             getppid());

      driver(&msg, root, broadcast);

      break;

    default:
      printf("Это родительский процесс\npid = %d\nppid = %d\n", getpid(),
             getppid());

      // memset(msg.mtext, '\000', sizeof(msg.mtext));
      ssize_t tmp = msgrcv(root, &msg, sizeof(msg), 1, 0);
      if (tmp == -1) {
        perror("msgrcv");
        exit(1);
      }

      printf("%d\n", msg.pid);

      break;
    }
    break;

  case '2':
    break;

  case '3':
    break;

  case '4':
    break;

  case '5':
    break;

  default:
    break;
  }

  msgctl(root, IPC_RMID, NULL);
  msgctl(broadcast, IPC_RMID, NULL);

  exit(0);
}