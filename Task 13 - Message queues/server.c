#include "list.h"
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
#include <unistd.h>

struct msgbuf {
  long mtype;
  char mtext[1024];
};

void send(int id, const char src[], struct msgbuf *msg, int typ, int flags) {
  memset(msg->mtext, '\000', sizeof(msg->mtext));
  memmove(msg->mtext, src, strlen(src));
  msg->mtype = typ;
  int tmp = msgsnd(id, msg, sizeof(msg->mtext), flags);
  if (tmp == -1) {
    perror("msgsnd");
    exit(EXIT_FAILURE);
  }
}

int receive(int id, struct msgbuf *msg, int typ, int flags) {
  memset(msg->mtext, '\000', sizeof(msg->mtext));
  ssize_t tmp = msgrcv(id, msg, sizeof(msg->mtext), typ, flags);
  if (tmp == -1) {
    if (errno != ENOMSG) {
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
    return 0;
  }
  return tmp;
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

  struct msgbuf msg;

  struct list *list = create();
  struct list *message_history = create();
  int count_message = 0;

  int stop = 1;
  while (stop) {
    if (receive(root, &msg, 1, IPC_NOWAIT)) {
      char pid[10] = {'\000'};
      int i, k = 0;
      for (i = 0; msg.mtext[i] != '/'; ++i) {
        pid[i] = msg.mtext[i];
      }
      i++;
      char name[1024] = {'\000'};
      while (msg.mtext[i] != '\000') {
        name[k++] = msg.mtext[i++];
      }

      long int id = strtol(pid, NULL, 10);
      if (lookup(list, name) != -1) {
        send(broadcast, "No-No-No", &msg, id, IPC_NOWAIT);
      } else {
        send(broadcast, "Yes", &msg, id, IPC_NOWAIT);
        push(list, id, name);
        printf("Connect - %s - %ld\n", get_name(list, lookup(list, name)), id);

        char client[1024] = {'\000'};
        snprintf(client, sizeof(client), "%d", list->size);

        struct node *tmp = list->head;
        while (tmp) {
          send(broadcast, client, &msg, tmp->value + 2, IPC_NOWAIT);
          tmp = tmp->next;
        }
        tmp = list->head;
        while (tmp) {
          struct node *temp = list->head;
          while (temp) {
            send(broadcast, tmp->name, &msg, temp->value + 2, IPC_NOWAIT);
            temp = temp->next;
          }
          tmp = tmp->next;
        }

        tmp = message_history->head;
        while (tmp) {
          usleep(1000000 / 4);
          send(broadcast, tmp->name, &msg, id, IPC_NOWAIT);
          tmp = tmp->next;
        }
      }
    }

    struct node *tmp = list->head;
    while (tmp) {
      if (receive(root, &msg, tmp->value, IPC_NOWAIT)) {
        printf("%s - %s\n", get_name(list, tmp->value), msg.mtext);
        char message[100] = {'\000'};
        strcat(message, get_name(list, tmp->value));
        strcat(message, "/");
        strcat(message, msg.mtext);
        push(message_history, count_message++, message);
        struct node *temp = list->head;
        for (; temp; temp = temp->next) {
          usleep(1000000 / 4);
          printf("(%d) - %s\n", temp->value, message);
          send(broadcast, message, &msg, temp->value, IPC_NOWAIT);
        }
      }

      if (receive(root, &msg, tmp->value + 1, IPC_NOWAIT)) {
        printf("Bay - %s\n", get_name(list, tmp->value));
        pop(list, tmp->value);
        if (!list->size)
          stop = 0;
        char client[1024] = {'\000'};
        snprintf(client, sizeof(client), "%d", list->size);

        struct node *tmp = list->head;
        while (tmp) {
          send(broadcast, client, &msg, tmp->value + 2, IPC_NOWAIT);
          tmp = tmp->next;
        }
        tmp = list->head;
        while (tmp) {
          struct node *temp = list->head;
          while (temp) {
            send(broadcast, tmp->name, &msg, temp->value + 2, IPC_NOWAIT);
            temp = temp->next;
          }
          tmp = tmp->next;
        }
        break;
      }
      tmp = tmp->next;
    }
  }

  delete (list);
  delete (message_history);
  msgctl(root, IPC_RMID, NULL);
  msgctl(broadcast, IPC_RMID, NULL);

  exit(0);
}