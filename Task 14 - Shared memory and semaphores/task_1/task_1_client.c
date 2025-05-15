#include "task_1_posix.h"
#include "task_1_sysetem_v.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>

void POSIX() {
  printf("POSIX\n");
  int fd;
  char shmpath[] = "/task_1", string[] = "Hello";
  size_t len;
  struct shmbuf *shmp;

  len = strlen(string);

  if (len > BUF_SIZE) {
    fprintf(stderr, "String is too long\n");
    exit(EXIT_FAILURE);
  }

  fd = shm_open(shmpath, O_RDWR, 0);
  if (fd == -1)
    errExit("shm_open");

  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED)
    errExit("mmap");

  shmp->cnt = len;
  memcpy(&shmp->buf, string, len);

  if (sem_post(&shmp->sem1) == -1)
    errExit("sem_post");

  if (sem_wait(&shmp->sem2) == -1)
    errExit("sem_wait");

  write(STDOUT_FILENO, &shmp->buf, len);
  write(STDOUT_FILENO, "\n", 1);
}

void SYSTEM_V() {
  printf("SYSTEM V\n");
  int semid, shmid;
  char *addr;
  size_t len;
  union semun arg;
  struct sembuf sop;

  char str[] = "Hello";

  key_t key = ftok("Makefile", 100);

  len = strlen(str) + 1;
  if (len > MEM_SIZE) {
    fprintf(stderr, "String is too big!\n");
    exit(EXIT_FAILURE);
  }

  shmid = shmget(key, MEM_SIZE, 0);
  if (shmid == -1)
    errExit("shmget");

  semid = semget(key, 1, 0);
  if (semid == -1)
    errExit("semget");

  addr = shmat(shmid, NULL, 0);
  if (addr == (void *)-1)
    errExit("shmat");

  memcpy(addr, str, len);

  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if (semop(semid, &sop, 1) == -1)
    errExit("semop");

  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == -1)
    errExit("semctl");

  sop.sem_num = 0;
  sop.sem_op = 0;
  sop.sem_flg = 0;

  if (semop(semid, &sop, 1) == -1)
    errExit("semop");

  printf("%s\n", addr);
}

int main() {

  POSIX();

  sleep(3);

  SYSTEM_V();

  exit(EXIT_SUCCESS);
}