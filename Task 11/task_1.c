#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *get_index(void *arg) {
  int *i = arg;
  printf("Я поток номер %d\n", *i);
  return NULL;
}

int main() {
  pthread_t thread[5];
  int index[] = {1, 2, 3, 4, 5};

  for (int i = 0; i < 5; ++i) {
    pthread_create(&thread[i], NULL, get_index, &index[i]);
  }

  for (int i = 0; i < 5; ++i) {
    pthread_join(thread[i], NULL);
  }

  return 0;
}