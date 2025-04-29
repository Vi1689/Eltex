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
  char name_qeuq[] = "/task_1";

  struct mq_attr attr;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10; // Максимальное количество сообщений
  attr.mq_msgsize = 100; // Максимальный размер сообщения
  attr.mq_curmsgs = 0;

  mqd_t qeuq = mq_open(name_qeuq, O_RDWR | O_CREAT, S_IRWXU, &attr);
  if (qeuq == (mqd_t)-1) {
    perror("mq_open");
    exit(1);
  }
  char send[] = "Hi!";

  mq_send(qeuq, send, 100, 2);

  char recv[100];
  memset(recv, '\000', 100);

  unsigned int prio = 0;

  while (1) {
    mq_receive(qeuq, recv, 100, &prio);
    if (prio == 1) {
      printf("%s\n", recv);
      break;
    } else {
      mq_send(qeuq, send, 100, 2);
    }
  }

  mq_close(qeuq);
  mq_unlink(name_qeuq);
}

void system_v() {
  printf("SYSTEM V\n");
  key_t key = ftok("Makefile", 100);

  int qeuq = msgget(key, 0666 | IPC_CREAT);
  if (qeuq == -1) {
    perror("msgget");
    exit(1);
  }

  struct msgbuf msg;

  msg.mtype = 1;
  memset(msg.mtext, '\000', 100);
  memmove(msg.mtext, "Hello!", sizeof("Hello!"));
  if (msgsnd(qeuq, &msg, sizeof(msg.mtext), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
  printf("sent: %s\n", msg.mtext);

  if (msgrcv(qeuq, &msg, sizeof(msg.mtext), 2, 0) == -1) {
    if (errno != ENOMSG) {
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
    printf("No message available for msgrcv()\n");
  } else {
    printf("message received: %s\n", msg.mtext);
  }
  msgctl(qeuq, IPC_RMID, NULL);
}

int main() {
  posix();

  system_v();

  exit(0);
}