#ifndef MY_PTHREAD_POOL
#define MY_PTHREAD_POOL

#include <pthread.h>

struct task {
  void *(*function)(void *);
  void *arg;
};

void *start_task();

void add_task(void *(*function)(void *), void *arg);

#endif