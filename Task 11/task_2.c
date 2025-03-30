#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t m1;

int count;

void *increment_mutex(void *arg) {
  int *a = arg;
  for (int i = 0; i < *a; ++i) {
    pthread_mutex_lock(&m1);
    count++;
    pthread_mutex_unlock(&m1);
  }
  return NULL;
}

void *increment(void *arg) {
  int *a = arg;
  int *local_count = malloc(sizeof(int));
  *local_count = 0;

  for (int i = 0; i < *a; ++i) {
    (*local_count)++;
  }

  return local_count;
}

int main() {
  int consistent = 0;
  for (int i = 0; i < 180000000; ++i) {
    consistent++;
  }

  pthread_t thread[8];
  int increment_value = 180000000 / 8;

  pthread_mutex_init(&m1, NULL);

  for (int i = 0; i < 8; ++i) {
    pthread_create(&thread[i], NULL, increment_mutex, &increment_value);
  }

  for (int i = 0; i < 8; ++i) {
    pthread_join(thread[i], NULL);
  }

  pthread_mutex_destroy(&m1);

  pthread_t thread_1[8];

  int mapreduce = 0;

  for (int i = 0; i < 8; ++i) {
    pthread_create(&thread_1[i], NULL, increment, &increment_value);
  }

  for (int i = 0; i < 8; ++i) {
    int *temp;
    pthread_join(thread_1[i], (void **)&temp);
    mapreduce += *temp;
    free(temp);
  }

  printf("Последовательная программа %d\nМногопоточная программа с mutex "
         "%d\nМногопоточная программа с локальными переменными %d\n",
         consistent, count, mapreduce);
}