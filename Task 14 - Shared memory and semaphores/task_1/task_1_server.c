#include "task_1_posix.h"
#include "task_1_sysetem_v.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void POSIX() {
  printf("POSIX\n");
  int fd;
  char shmpath[] = "/task_1";
  struct shmbuf *shmp;

  fd = shm_open(shmpath, O_CREAT | O_EXCL | O_RDWR, 0600);
  if (fd == -1)
    errExit("shm_open");

  if (ftruncate(fd, sizeof(struct shmbuf)) == -1)
    errExit("ftruncate");

  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED)
    errExit("mmap");

  if (sem_init(&shmp->sem1, 1, 0) == -1)
    errExit("sem_init-sem1");
  if (sem_init(&shmp->sem2, 1, 0) == -1)
    errExit("sem_init-sem2");

  if (sem_wait(&shmp->sem1) == -1)
    errExit("sem_wait");

  write(STDOUT_FILENO, &shmp->buf, shmp->cnt);
  write(STDOUT_FILENO, "\n", 1);

  memset(&shmp->buf, '\000', sizeof(shmp->buf));
  memcpy(&shmp->buf, "Hi", strlen("Hi"));

  shmp->cnt = strlen("Hi");

  if (sem_post(&shmp->sem2) == -1)
    errExit("sem_post");

  munmap(NULL, sizeof(*shmp));
  shm_unlink(shmpath);
}

void SYSTEM_V() {
  printf("SYSTEM V\n");

  int semid, shmid;
  char *addr;
  union semun arg, dummy;
  struct sembuf sop;

  key_t key = ftok("Makefile", 100);

  shmid = shmget(key, MEM_SIZE, IPC_CREAT | 0600);
  if (shmid == -1)
    errExit("shmget");

  semid = semget(key, 1, IPC_CREAT | 0600);
  if (semid == -1)
    errExit("semget");

  addr = shmat(shmid, NULL, 0);
  if (addr == (void *)-1)
    errExit("shmat");

  arg.val = 1;
  if (semctl(semid, 0, SETVAL, arg) == -1)
    errExit("semctl");

  sop.sem_num = 0;
  sop.sem_op = 0;
  sop.sem_flg = 0;

  if (semop(semid, &sop, 1) == -1)
    errExit("semop");

  printf("%s\n", addr);

  memset(addr, '\000', strlen(addr));
  memcpy(addr, "Hi", strlen("Hi"));

  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if (semop(semid, &sop, 1) == -1)
    errExit("semop");

  if (shmctl(shmid, IPC_RMID, NULL) == -1)
    errExit("shmctl");
  if (semctl(semid, 0, IPC_RMID, dummy) == -1)
    errExit("semctl");
}

int main() {

  POSIX();

  SYSTEM_V();

  exit(EXIT_SUCCESS);
}