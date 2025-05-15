#include "list.h"
#include "posix.h"
#include <curses.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

void sig_winch() {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

int end = 1;

struct arg {
  char *name;
  struct list *names;
  struct shmbuf *shmp;
  WINDOW *sub_user_names, *sub_user_messages;
};

void *fun(void *arg) {
  struct arg *a = arg;
  struct arg b = *a;
  struct shmbuf *shmp = b.shmp;
  struct list *names = b.names;
  char *my_name = b.name;
  WINDOW *sub_user_messages = b.sub_user_messages,
         *sub_user_names = b.sub_user_names;

  while (end) {
    sem_wait(&shmp->count);

    char action[MASSEGE_SIZE] = {'\000'}, name[NAME_SIZE] = {'\000'},
         temp[MASSEGE_SIZE] = {'\000'};
    memcpy(temp, shmp->name, shmp->size_name);

    int i, k = 0;
    for (i = 0; temp[i] != '/'; ++i) {
      action[k++] = temp[i];
    }
    i++;
    k = 0;
    for (; temp[i] != '\000'; ++i) {
      name[k++] = temp[i];
    }

    if (strcmp(action, "message") == 0) {
      wprintw(sub_user_messages, "%s: %s\n", name, shmp->message);
      wrefresh(sub_user_messages);
    } else if (strcmp(action, "exit") == 0) {
      pop(names, name);
      struct node *tmp = names->head;
      while (tmp) {
        printf("%s\n", tmp->name);
        tmp = tmp->next;
      }
    } else if (strcmp(action, "new") == 0) {
      wclear(sub_user_names);
      if ((lookup(names, name)))
        push(names, 0, name);
      struct node *tmp = names->head;
      while (tmp) {
        wprintw(sub_user_names, "%s\n", tmp->name);
        wrefresh(sub_user_names);
        tmp = tmp->next;
      }
    } else if (strcmp(action, my_name) == 0) {
      wprintw(sub_user_messages, "%s: %s\n", name, shmp->message);
      wrefresh(sub_user_messages);
    } else {
      sem_post(&shmp->count);
      sem_post(&shmp->number_chats);
    }
    sem_wait(&shmp->number_chats);
  }

  return NULL;
}

int main(int argc, char *argv[]) {

  WINDOW *user_messages, *sub_user_messages;
  WINDOW *user_names, *sub_user_names;
  WINDOW *message_input, *sub_message_input;

  if (argc <= 1) {
    perror("None arg");
    exit(1);
  }

  int fd;
  char shmpath[] = "/queue";

  if (strlen(argv[1]) > NAME_SIZE) {
    perror("Long name");
    exit(1);
  }

  struct shmbuf *shmp;

  fd = shm_open(shmpath, O_RDWR, 0);
  if (fd == -1) {
    errExit("shm_open");
  }

  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED) {
    errExit("mmap");
  }

  int value = 0;

  while (1) {
    sem_getvalue(&shmp->sem3, &value);
    if (!value) {
      sem_post(&shmp->sem3);
      break;
    }
  }

  memset(shmp->name, '\000', sizeof(shmp->name));
  memset(shmp->message, '\000', sizeof(shmp->message));

  char shmname[] = "connect/";
  strcat(shmname, argv[1]);
  shmp->size_name = strlen(shmname);
  shmp->size_message = strlen("connect");
  memcpy(shmp->name, shmname, shmp->size_name);
  memcpy(shmp->message, "connect", shmp->size_message);
  sem_post(&shmp->sem1);
  sem_wait(&shmp->sem2);

  if (strcmp(shmp->message, "no") == 0) {
    sem_wait(&shmp->sem3);
    printf("This name is occupied");
    exit(0);
  } else if (strcmp(shmp->message, "yes") == 0) {
    // histori
    sem_wait(&shmp->sem3);
  } else {
    sem_wait(&shmp->sem3);
    perror("Error");
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

  char input[MASSEGE_SIZE];

  struct list *names = create();

  pthread_t th;

  struct arg arg = {argv[1], names, shmp, sub_user_names, sub_user_messages};

  pthread_create(&th, NULL, fun, &arg);

  while (1) {
    memset(input, '\000', sizeof(input));
    echo();
    wgetnstr(sub_message_input, input, sizeof(input) - 1);
    noecho();
    // fgets(input, sizeof(input), stdin);
    if (strlen(input) == 0)
      continue;

    if (strcmp(input, "exit") == 0) {
      while (1) {
        sem_getvalue(&shmp->sem3, &value);
        if (!value) {
          sem_post(&shmp->sem3);
          break;
        }
      }

      memset(shmp->name, '\000', sizeof(shmp->name));
      memset(shmp->message, '\000', sizeof(shmp->message));

      char shmname[] = "exit/";
      strcat(shmname, argv[1]);
      shmp->size_name = strlen(shmname);
      shmp->size_message = strlen("exit");
      memcpy(shmp->name, shmname, strlen(shmname));
      memcpy(shmp->message, "exit", shmp->size_message);
      sem_post(&shmp->sem1);
      sem_wait(&shmp->sem2);
      sem_wait(&shmp->sem3);
      end = 0;
      pthread_join(th, NULL);
      break;
    }

    while (1) {
      sem_getvalue(&shmp->sem3, &value);
      if (!value) {
        sem_post(&shmp->sem3);
        break;
      }
    }

    memset(&shmp->message, '\000', shmp->size_message);
    shmp->size_message = strlen(input);
    memcpy(&shmp->message, input, shmp->size_message);

    memset(&shmp->name, '\000', shmp->size_message);
    char shmname[] = "message/";
    strcat(shmname, argv[1]);
    shmp->size_name = strlen(shmname);
    memcpy(&shmp->name, shmname, shmp->size_name);

    sem_post(&shmp->sem1);
    sem_wait(&shmp->sem3);
    sem_wait(&shmp->sem2);
    wclear(sub_message_input);
  }
  printf("Client end\n");
  delete (names);
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