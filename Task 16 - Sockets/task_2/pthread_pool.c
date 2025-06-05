#include "pthread_pool.h"

struct task queq[100];
int current_el = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *start_task() {
  while (1) {
    pthread_mutex_lock(&mutex);
    while (current_el == 0) {
      pthread_cond_wait(&cond, &mutex);
    }

    struct task tmp = queq[--current_el];

    pthread_mutex_unlock(&mutex);

    tmp.function(tmp.arg);
  }

  return NULL;
}

void add_task(void *(*function)(void *), void *arg) {
  pthread_mutex_lock(&mutex);
  queq[current_el++] = (struct task){function, arg};
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}