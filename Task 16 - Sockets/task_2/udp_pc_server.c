#include "pthread_pool.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

struct arg {
  int fd;
  struct sockaddr_in client;
};

void *fun(void *arg) {

  struct arg *a = arg;
  struct arg b = *a;

  int fd = b.fd;

  struct sockaddr_in client = b.client;

  char buf[100] = {'\000'};

  time_t t = time(NULL);
  struct tm *tm_info = localtime(&t);

  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);

  if ((sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client,
              sizeof(client))) == -1) {
    perror("Error - send\n");
    pthread_exit(NULL);
  }

  printf("Send - %s\n", buf);
  return NULL;
}

int main() {

  pthread_t pool[4];

  for (int i = 0; i < 4; ++i) {
    pthread_create(&pool[i], NULL, start_task, NULL);
  }

  struct sockaddr_in serv;

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd == -1) {
    perror("Error - socket\n");
    return 1;
  }

  memset(&serv, 0, sizeof(serv));

  serv.sin_family = AF_INET;
  serv.sin_port = htons(40011);
  if (inet_aton("127.0.0.11", &serv.sin_addr) == 0) {
    perror("Error - inet_aton");
    return 1;
  }

  if ((bind(fd, (struct sockaddr *)&serv, sizeof(serv))) == -1) {
    perror("Error - bind\n");
    return 1;
  }

  while (1) {
    struct sockaddr_in client;

    char buf[100] = {'\000'};
    socklen_t len = sizeof(client);
    if ((recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len)) ==
        -1) {
      perror("Error - recv\n");
      pthread_exit(NULL);
    }

    printf("Recv - %s\n", buf);

    struct arg arg = {fd, client};

    add_task(fun, &arg);
  }

  for (int i = 0; i < 4; ++i) {
    pthread_join(pool[i], NULL);
  }

  close(fd);

  return 0;
}