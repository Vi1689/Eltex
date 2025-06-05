#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

struct shmbuf {
  sem_t sem1;
  sem_t sem2;
  size_t cnt;
  char buf[1024];
};

void *fun() {
  int fd;
  char shmpath[] = "/qeuq";
  struct shmbuf *shmp;

  fd = shm_open(shmpath, O_RDWR, 0);
  if (fd == -1) {
    perror("Error - shm_open");
    pthread_exit(NULL);
  }

  shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmp == MAP_FAILED) {
    perror("Error - mmap");
    pthread_exit(NULL);
  }
  while (1) {

    if (sem_wait(&shmp->sem2) == -1) {
      perror("Error - sem_wait");
      pthread_exit(NULL);
    }

    int new_fd = strtol(shmp->buf, NULL, 10);

    char buf[100] = {'\000'};

    if ((recv(new_fd, buf, sizeof(buf), 0)) == -1) {
      perror("Error - recv");
      pthread_exit(NULL);
    }

    printf("Recv - %s\n", buf);

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

    if ((send(new_fd, buf, sizeof(buf), 0)) == -1) {
      perror("Error - send");
      pthread_exit(NULL);
    }

    printf("Send - %s\n", buf);

    close(new_fd);

    if (sem_post(&shmp->sem1) == -1) {
      perror("Error - sem_post");
      pthread_exit(NULL);
    }
  }
  return NULL;
}

int main() {

  char path[] = "/qeuq";

  shm_unlink(path);

  int fd_ipc = shm_open(path, O_CREAT | O_EXCL | O_RDWR, 0600);
  if (fd_ipc == -1) {
    perror("Error - shm_open");
    return 1;
  }

  if (ftruncate(fd_ipc, sizeof(struct shmbuf)) == -1) {
    perror("Error - ftruncate");
    return 1;
  }

  struct shmbuf *shmp;

  shmp =
      mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd_ipc, 0);
  if (shmp == MAP_FAILED) {
    perror("Error - mmap");
    return 1;
  }

  if (sem_init(&shmp->sem1, 1, 0) == -1) {
    perror("Error - sem1");
    return 1;
  }
  if (sem_init(&shmp->sem2, 1, 0) == -1) {
    perror("Error - sem2");
    return 1;
  }

  int number_th = 4;

  pthread_t pool[number_th];

  for (int i = 0; i < number_th; ++i) {
    pthread_create(&pool[i], NULL, fun, NULL);
  }

  struct sockaddr_in serv, client;

  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("Error - socket");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));

  serv.sin_family = AF_INET;
  serv.sin_port = htons(40010);
  if (inet_aton("127.0.0.10", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  printf("Port - %d\nIP - %s\n", htons(serv.sin_port),
         inet_ntoa(serv.sin_addr));

  if (bind(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
    perror("Error - bind");
    return 1;
  }

  if ((listen(fd, 5)) == -1) {
    perror("Error - listen");
    return 1;
  }

  while (1) {
    socklen_t len = sizeof(client);
    int new_fd = accept(fd, (struct sockaddr *)&client, &len);

    if (new_fd == -1) {
      perror("Error - accept");
      return 1;
    }
    if (sem_post(&shmp->sem2) == -1) {
      perror("Error - sem_post");
      return 1;
    }
    memset(&shmp->buf, '\000', sizeof(shmp->buf));
    char tmp[1024] = {'\000'};
    snprintf(tmp, sizeof(tmp), "%d", new_fd);
    memcpy(&shmp->buf, tmp, strlen(tmp));
    if (sem_wait(&shmp->sem1) == -1) {
      perror("Error - sem_wait");
      return 1;
    }
  }

  for (int i = 0; i < number_th; ++i) {
    pthread_join(pool[i], NULL);
  }

  munmap(NULL, sizeof(*shmp));
  shm_unlink(path);
  close(fd);

  return 0;
}