#include <stdio.h>

int main() {
  int a;
  printf("Введите число\na = ");
  scanf("%d", &a);
  printf("\n");

  int b;
  printf("Введите число от 0 до 255\nb = ");
  scanf("%d", &b);
  printf("\n");

  char *temp = (char *)&a;
  temp += 2;
  *temp = b;
  printf("a = %d\n", a);
  return 0;
}