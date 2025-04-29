#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

struct msgbuf {
  long mtype;
  char mtext[100];
};

void posix() {
  printf("POSIX\n");
  mqd_t qeuq = mq_open("/task_1", O_RDWR);
  if (qeuq == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }
  char resv[100];
  memset(resv, '\000', 100);

  unsigned int prio = 0;
  mq_receive(qeuq, resv, 100, &prio);
  if (prio == 2) {
    printf("%s\n", resv);
  } else {
    perror("mq_receive");
    mq_close(qeuq);
    exit(1);
  }

  char send[] = "Hello!";
  mq_send(qeuq, send, 100, 1);
  mq_close(qeuq);
}

void system_v() {
  printf("SYSTEM V\n");
  key_t key = ftok("Makefile", 100);

  int qeuq = msgget(key, 0);
  if (qeuq == -1) {
    perror("msgget");
    exit(1);
  }

  struct msgbuf msg;

  if (msgrcv(qeuq, &msg, sizeof(msg.mtext), 1, 0) == -1) {
    if (errno != ENOMSG) {
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
    printf("No message available for msgrcv()\n");
  } else {
    printf("message received: %s\n", msg.mtext);
  }

  msg.mtype = 2;
  memset(msg.mtext, '\000', 100);
  memmove(msg.mtext, "Hi!", sizeof("Hi!"));
  if (msgsnd(qeuq, &msg, sizeof(msg.mtext), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
  printf("sent: %s\n", msg.mtext);
}

int main() {
  posix();

  sleep(3);

  system_v();

  exit(0);
}