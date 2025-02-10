#include "calc.h"
#include <stdio.h>

int main() {

  int vvod, flag_stop = 1;

  while (flag_stop) {
    printf("1) Сложение\n2) Вычитание\n3) Умножение\n4) Деление\n5) Выход\n");
    scanf("%d", &vvod);
    int a, b;
    switch (vvod) {
    case 1:
      printf("Введите a\n");
      scanf("%d", &a);
      printf("Введите b\n");
      scanf("%d", &b);
      printf("Ответ: %d\n", add(a, b));
      break;
    case 2:
      printf("Введите a\n");
      scanf("%d", &a);
      printf("Введите b\n");
      scanf("%d", &b);
      printf("Ответ: %d\n", sub(a, b));
      break;
    case 3:
      printf("Введите a\n");
      scanf("%d", &a);
      printf("Введите b\n");
      scanf("%d", &b);
      printf("Ответ: %d\n", mul(a, b));
      break;
    case 4:
      printf("Введите a\n");
      scanf("%d", &a);
      printf("Введите b\n");
      scanf("%d", &b);
      int c = div(a, b);
      if (c == 0) {
        printf("Нельзя делить на ноль\n");
        break;
      }
      printf("Ответ: %d\n", div(a, b));
      break;
    case 5:
      flag_stop = 0;
      break;
    default:
      printf("Введите снова\n");
      break;
    }
  }
}