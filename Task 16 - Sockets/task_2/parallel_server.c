#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

void *fun(void *arg) {

  int *tmp = arg, new_fd = *tmp;

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
  pthread_exit(NULL);
}

int main() {

  pthread_t th;
  pthread_attr_t ta;

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

  pthread_attr_init(&ta);
  pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);

  while (1) {
    socklen_t len = sizeof(client);
    int new_fd = accept(fd, (struct sockaddr *)&client, &len);

    if (new_fd == -1) {
      perror("Error - accept");
      return 1;
    }

    if (pthread_create(&th, &ta, fun, &new_fd) < 0) {
      perror("Error - pthread_create");
      return 1;
    }
  }

  close(fd);

  return 0;
}