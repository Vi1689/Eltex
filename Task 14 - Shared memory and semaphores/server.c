#include "list.h"
#include "posix.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {

  int fd;
  char shmpath[] = "/queue";
  struct shmbuf *shmp;

  fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0600);
  if (fd == -1) {
    shm_unlink(shmpath);
    errExit("shm_open");
  }

  if (ftruncate(fd, sizeof(struct shmbuf)) == -1) {
    errExit("ftruncate");
  }

  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED) {
    errExit("mmap");
  }

  if (sem_init(&shmp->sem1, 1, 0) == -1) {
    errExit("sem_init-sem1");
  }
  if (sem_init(&shmp->sem2, 1, 0) == -1) {
    errExit("sem_init-sem2");
  }
  if (sem_init(&shmp->sem3, 1, 0) == -1) {
    errExit("sem_init-sem3");
  }
  if (sem_init(&shmp->number_chats, 1, 0) == -1) {
    errExit("sem_init-number_chats");
  }
  if (sem_init(&shmp->count, 1, 0) == -1) {
    errExit("sem_init-count");
  }

  struct list *names = create();
  struct list *messages = create();

  while (1) {
    int value = 0;
    sem_getvalue(&shmp->sem1, &value);

    if (value) {
      sem_wait(&shmp->sem1);
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

      if (strcmp(action, "connect") == 0) {
        if (lookup(names, name) == 0) {
          memset(shmp->message, '\000', shmp->size_message);

          shmp->size_message = strlen("no");
          memcpy(shmp->message, "no", shmp->size_message);

          sem_post(&shmp->sem2);
        } else {
          printf("%s: %s\n", temp, shmp->message);
          push(names, 0, name);
          memset(shmp->message, '\000', shmp->size_message);
          shmp->size_message = strlen("yes");
          memcpy(shmp->message, "yes", shmp->size_message);
          sem_post(&shmp->sem2);

          while (1) {
            sem_getvalue(&shmp->sem3, &value);
            if (!value) {
              sem_post(&shmp->sem3);
              break;
            }
          }

          struct node *tmp = names->head;
          while (tmp) {
            memset(shmp->name, '\000', sizeof(shmp->name));
            memset(shmp->message, '\000', sizeof(shmp->message));

            char shmname[] = "new/";
            strcat(shmname, tmp->name);
            shmp->size_name = strlen(shmname);
            shmp->size_message = strlen("new");
            memcpy(shmp->name, shmname, shmp->size_name);
            memcpy(shmp->message, "new", shmp->size_message);

            for (int i = 0; i < names->size; ++i) {
              sem_post(&shmp->count);
            }
            sleep(2);
            for (int i = 0; i < names->size; ++i) {
              sem_post(&shmp->number_chats);
            }

            tmp = tmp->next;
          }

          tmp = messages->head;
          while (tmp) {
            memset(shmp->name, '\000', sizeof(shmp->name));
            memset(shmp->message, '\000', sizeof(shmp->message));

            char shmname[NAME_SIZE] = {'\000'};
            strcat(shmname, name);
            strcat(shmname, "/");

            char message[MASSEGE_SIZE] = {'\000'},
                 sub_name[NAME_SIZE] = {'\000'};
            int i, k = 0;
            for (i = 0; tmp->name[i] != '/'; ++i) {
              sub_name[k++] = tmp->name[i];
            }
            i++;
            k = 0;
            for (; tmp->name[i] != '\000'; ++i) {
              message[k++] = tmp->name[i];
            }

            strcat(shmname, sub_name);
            shmp->size_name = strlen(shmname);
            shmp->size_message = strlen(message);
            memcpy(shmp->name, shmname, shmp->size_name);
            memcpy(shmp->message, message, shmp->size_message);

            printf("%s %s\n", shmname, shmp->message);

            sem_post(&shmp->count);
            sleep(2);
            sem_post(&shmp->number_chats);

            tmp = tmp->next;
          }

          sem_wait(&shmp->sem3);
        }
      } else if (strcmp(action, "exit") == 0) {
        sem_post(&shmp->sem2);

        printf("%s: %s\n", temp, shmp->message);
        while (1) {
          sem_getvalue(&shmp->sem3, &value);
          if (!value) {
            sem_post(&shmp->sem3);
            break;
          }
        }

        for (int i = 0; i < names->size; ++i) {
          sem_post(&shmp->count);
        }
        sleep(2);
        for (int i = 0; i < names->size; ++i) {
          sem_post(&shmp->number_chats);
        }

        sem_wait(&shmp->sem3);
        pop(names, name);
        if (names->size == 0) {
          break;
        }
      } else {
        while (1) {
          sem_getvalue(&shmp->sem3, &value);
          if (!value) {
            sem_post(&shmp->sem3);
            break;
          }
        }

        char keeping[MASSEGE_SIZE] = {'\000'};

        strcat(keeping, name);
        strcat(keeping, "/");
        strcat(keeping, shmp->message);

        push(messages, 0, keeping);

        printf("%s: %s\n", temp, shmp->message);

        for (int i = 0; i < names->size; ++i) {
          sem_post(&shmp->count);
        }
        sleep(2);
        for (int i = 0; i < names->size; ++i) {
          sem_post(&shmp->number_chats);
        }

        sem_post(&shmp->sem2);
        sem_wait(&shmp->sem3);
      }
    }
  }
  delete (names);
  delete (messages);
  shm_unlink(shmpath);
  exit(0);
}