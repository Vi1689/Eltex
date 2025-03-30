#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct room {
  int product;
  char loose;
  // '0' свободный
  // '1' занят
};

int end_shop = 1;

struct room shop[] = {
    {500, '0'}, {500, '0'}, {500, '0'}, {500, '0'}, {500, '0'}};

pthread_mutex_t m[5];

void *buyer(void *arg) {
  unsigned int *seed = arg, id = *seed;
  int requirement = 10000 + rand_r(seed) % 2000;
  while (requirement > 0) {
    sleep(2);
    printf("Я покупатель номер %d и я проснулся. Вот моя потребность %d\n", id,
           requirement);
    for (int i = 0; i < 5; ++i) {
      if (shop[i].loose == '0') {
        if (shop[i].product == 0) {
          continue;
        }
        pthread_mutex_lock(&m[i]);
        shop[i].loose = '1';
        requirement -= shop[i].product;
        printf("Я покупатель номер %d и я зашёл в магазин %d, там столько "
               "товара %d. Вот моя потребность %d и я засыпаю\n",
               id, i, shop[i].product, requirement);
        shop[i].product = 0;
        if (requirement < 0) {
          shop[i].product = requirement * -1;
        }
        shop[i].loose = '0';
        pthread_mutex_unlock(&m[i]);
        break;
      }
    }
  }
  printf("Я покупатель %d и я закончил покупки\n", id);
}

void *loader(void *arg) {
  unsigned int *id = arg;
  int a = 0;
  while (end_shop) {
    sleep(1);
    printf("Я погрузчик %d и я проснулся\n", *id);
    for (int i = 0; i < 5; ++i) {
      a++;
      if (a == 5) {
        a = 0;
      }
      if (shop[a].loose == '0') {
        pthread_mutex_lock(&m[a]);
        shop[a].loose = '1';
        shop[a].product += 500;
        printf("Я погрузчик %d и я пополнил магазин %d на 500 товара, теперь "
               "там %d и я засыпаю\n",
               *id, a, shop[a].product);
        shop[a].loose = '0';
        pthread_mutex_unlock(&m[a]);
        break;
      }
    }
  }
  printf("Я погрузчик %d и я закончил\n", *id);
}

int main() {
  srand(time(NULL));
  pthread_t buyers[3], loaders;
  unsigned int id[] = {1, 2, 3, 4};

  for (int i = 0; i < 5; ++i) {
    pthread_mutex_init(&m[i], NULL);
  }

  for (int i = 0; i < 3; ++i) {
    pthread_create(&buyers[i], NULL, buyer, &id[i]);
  }
  pthread_create(&loaders, NULL, loader, &id[3]);

  for (int i = 0; i < 3; ++i) {
    pthread_join(buyers[i], NULL);
  }
  end_shop = 0;
  pthread_join(loaders, NULL);

  for (int i = 0; i < 5; ++i) {
    pthread_mutex_destroy(&m[i]);
  }

  return 0;
}