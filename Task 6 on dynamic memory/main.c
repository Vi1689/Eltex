#include "my_list.h"
#include <stdio.h>

int main() {
  int stop = 1;
  struct root *guide = create();
  while (stop) {
    printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по "
           "имени\n4) Вывод всех записей\n5) Выход\n");
    int input;
    scanf("%d", &input);
    struct abonent a;
    switch (input) {
    case 1:
      printf("Введите имя абонента\n");
      scanf("%s", a.name);
      printf("Введите фамилию абонента\n");
      scanf("%s", a.second_name);
      printf("Введите телефон абонента\n");
      scanf("%s", a.tel);
      add(guide, a);
      break;
    case 2:
      printf("Введите имя абонента для удаления\n");
      scanf("%s", a.name);
      printf("Введите фамилию абонента для удаления\n");
      scanf("%s", a.second_name);
      printf("Введите телефон абонента для удаления\n");
      scanf("%s", a.tel);
      delete (guide, a);
      break;
    case 3:
      char find_name[10];
      printf("Введите имя абонента для поиска\n");
      scanf("%s", find_name);
      find(guide, find_name);
      break;
    case 4:
      print(guide);
      break;
    case 5:
      stop = 0;
      free_list(guide);
      break;
    default:
      printf("Некорректный ввод\n");
      break;
    }
  }
}