#include <stdio.h>

void task_1_2(int a) {
  for (int i = sizeof(a) * 8 - 1; i >= 0; --i) {
    printf("%d", a >> i & 1);
  }
  printf("\n");
}

void task_3(int a) {
  int count = 0;
  for (int i = sizeof(a) * 8 - 1; i >= 0; --i) {
    count += (a >> i & 1);
  }
  printf("%d\n", count);
}

void task_4(int a, int b) {
  a &= 0xFF00FFFF;
  b <<= 16;
  a += b;
  task_1_2(a);
  printf("%d\n", a);
}

int main() {
  int a;
  printf("Введите положительное число a = ");
  scanf("%d", &a);
  task_1_2(a);
  printf("Введите отрицательное число a = ");
  scanf("%d", &a);
  task_1_2(a);
  printf("Введите положительное число a = ");
  scanf("%d", &a);
  task_3(a);
  int b;
  printf("Введите положительное число a = ");
  scanf("%d", &a);
  printf("Введите число(0-255) b = ");
  scanf("%d", &b);
  task_4(a, b);
  return 0;
}