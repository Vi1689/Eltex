#ifndef my_posix__
#define my_posix__

#include "list.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define errExit(msg)                                                           \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define MASSEGE_SIZE 1024
#define NAME_SIZE 100

struct shmbuf {
  sem_t sem1; // send server
  sem_t sem2; // recv server
  sem_t sem3; // mutex client
  sem_t number_chats;
  sem_t count;
  size_t size_message;
  char message[MASSEGE_SIZE];
  size_t size_name;
  char name[NAME_SIZE];
};

#endif