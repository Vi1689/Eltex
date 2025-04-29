#include "list.h"
#include <curses.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

struct msgbuf {
  long mtype;
  char mtext[1024];
};

struct arg {
  int broadcast, id_proi;
  struct msgbuf msg;
  struct list *list;
  WINDOW *sub_user_names, *sub_user_messages;
};

int end = 1;

void send(int id, const char src[], struct msgbuf *msg, int typ, int flags) {
  memset(msg->mtext, '\000', sizeof(msg->mtext));
  memmove(msg->mtext, src, strlen(src));
  msg->mtype = typ;
  int tmp = msgsnd(id, msg, sizeof(msg->mtext), flags);
  if (tmp == -1) {
    perror("msgrcv");
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

void *fun(void *arg) {
  struct arg *a = arg;
  struct arg b = *a;
  int broadcast = b.broadcast, id_proi = b.id_proi;
  struct msgbuf msg = b.msg;
  struct list *list = b.list;
  WINDOW *sub_user_messages = b.sub_user_messages,
         *sub_user_names = b.sub_user_names;
  while (end) {
    if (receive(broadcast, &msg, id_proi, IPC_NOWAIT)) {
      int i = 0, k = 0;
      char name[1024] = {'\000'}, message[1024] = {'\000'};
      while (msg.mtext[i] != '/') {
        name[k++] = msg.mtext[i++];
      }
      i++;
      k = 0;
      while (msg.mtext[i] != '\000') {
        message[k++] = msg.mtext[i++];
      }
      wprintw(sub_user_messages, "%s: %s\n", name, message);
      wrefresh(sub_user_messages);
      // printf("%s: %s\n", name, message);
    }

    if (receive(broadcast, &msg, id_proi + 2, IPC_NOWAIT)) {
      wclear(sub_user_names);
      clear_list(list);
      int size = strtol(msg.mtext, NULL, 10);
      for (int i = 0; i < size; ++i) {
        if (receive(broadcast, &msg, id_proi + 2, 0)) {
          push(list, i, msg.mtext);
        }
      }
      struct node *tmp = list->head;
      for (int i = 0; i < list->size; ++i, tmp = tmp->next) {
        wprintw(sub_user_names, "%s\n", tmp->name);
        wrefresh(sub_user_names);
        // printf("%s\n", tmp->name);
      }
    }
  }
  return NULL;
}

void sig_winch() {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char *argv[]) {

  WINDOW *user_messages, *sub_user_messages;
  WINDOW *user_names, *sub_user_names;
  WINDOW *message_input, *sub_message_input;

  key_t key_root = ftok("Makefile", 100),
        key_broadcast = ftok("Makefile", 1000);

  int root = msgget(key_root, 0);
  if (root == -1) {
    perror("msgget root");
    exit(1);
  }

  int broadcast = msgget(key_broadcast, 0);
  if (broadcast == -1) {
    perror("msgget broadcast");
    exit(1);
  }

  int id_proi = getpid() * 10;

  struct list *list = create();

  char name[1024] = {'\000'};
  snprintf(name, sizeof(name), "%d", id_proi);
  struct msgbuf msg;
  if (argc > 1) {
    strcat(name, "/");
    strcat(name, argv[1]);
    send(root, name, &msg, 1, IPC_NOWAIT);

    if (receive(broadcast, &msg, id_proi, 0)) {
      if (strcmp(msg.mtext, "No-No-No") == 0) {
        printf("This name is occupied\n");
        exit(0);
      } else {
        printf("You are in the chat - %s\n", argv[1]);
      }
    }

  } else {
    perror("None arg");
    exit(1);
  }

  initscr();
  signal(SIGWINCH, sig_winch);
  curs_set(FALSE);
  start_color();
  refresh();

  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

  user_messages = newwin(size.ws_row * 0.9, size.ws_col * 0.75 - 1, 0, 0);
  box(user_messages, '|', '-');

  user_names =
      newwin(size.ws_row * 0.9, size.ws_col * 0.25, 0, size.ws_col * 0.75);
  box(user_names, '|', '-');

  message_input = newwin(size.ws_row * 0.1, size.ws_col, size.ws_row * 0.9, 0);
  box(message_input, '|', '-');

  sub_user_messages = derwin(user_messages, size.ws_row * 0.9 - 2,
                             size.ws_col * 0.75 - 3, 1, 1);
  sub_user_names =
      derwin(user_names, size.ws_row * 0.9 - 2, size.ws_col * 0.25 - 2, 1, 1);
  sub_message_input =
      derwin(message_input, size.ws_row * 0.1 - 2, size.ws_col - 2, 1, 1);

  scrollok(sub_user_messages, TRUE);

  wrefresh(user_messages);
  wrefresh(sub_user_messages);
  wrefresh(user_names);
  wrefresh(sub_user_names);
  wrefresh(message_input);
  wrefresh(sub_message_input);

  char input[1024];

  pthread_t th;
  struct msgbuf msg_output = {1, "\000"};
  struct arg arg;
  arg.broadcast = broadcast;
  arg.id_proi = id_proi;
  arg.msg = msg_output;
  arg.list = list;
  arg.sub_user_messages = sub_user_messages;
  arg.sub_user_names = sub_user_names;

  pthread_create(&th, NULL, fun, &arg);

  keypad(sub_message_input, TRUE);

  while (1) {
    memset(input, '\000', sizeof(input));
    echo();
    wgetnstr(sub_message_input, input, sizeof(input) - 1);
    noecho();
    // fgets(input, sizeof(input), stdin);
    if (strlen(input) == 0)
      continue;

    if (strcmp(input, "exit") == 0) {
      end = 0;
      pthread_join(th, NULL);
      send(root, input, &msg, id_proi + 1, IPC_NOWAIT);
      break;
    }
    send(root, input, &msg, id_proi, IPC_NOWAIT);
    wclear(sub_message_input);
  }
  delete (list);
  delwin(user_messages);
  delwin(sub_user_messages);
  delwin(user_names);
  delwin(sub_user_names);
  delwin(message_input);
  delwin(sub_message_input);
  refresh();
  getch();
  endwin();
  exit(0);
}