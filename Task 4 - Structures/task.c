#include <stdio.h>
#include <string.h>

struct abonent {
  char name[10];
  char second_name[10];
  char tel[10];
};

int main() {
  int stop = 1, size = 0;
  struct abonent guide[100];

  while (stop) {
    printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по "
           "имени\n4) Вывод всех записей\n5) Выход\n");
    int input;
    scanf("%d", &input);
    switch (input) {
    case 1:
      if (size == 100) {
        printf("Справочник переполнен, больше нельзя записать абонентов\n");
        break;
      }
      printf("Введите имя абонента\n");
      scanf("%s", guide[size].name);
      printf("Введите фамилию абонента\n");
      scanf("%s", guide[size].second_name);
      printf("Введите телефон абонента\n");
      scanf("%s", guide[size].tel);
      size++;
      break;
    case 2:
      char delete_name[10], delete_second_name[10], delete_tel[10];
      printf("Введите имя абонента для удаления\n");
      scanf("%s", delete_name);
      printf("Введите фамилию абонента для удаления\n");
      scanf("%s", delete_second_name);
      printf("Введите телефон абонента для удаления\n");
      scanf("%s", delete_tel);
      for (int i = 0; i < size; ++i) {
        if (!strcmp(delete_name, guide[i].name) &&
            !strcmp(delete_second_name, guide[i].second_name) &&
            !strcmp(delete_tel, guide[i].tel)) {
          memcpy(guide[i].name, "000000000", 10);
          memcpy(guide[i].second_name, "000000000", 10);
          memcpy(guide[i].tel, "000000000", 10);
        }
      }
      break;
    case 3:
      char find_name[10];
      printf("Введите имя абонента для поиска\n");
      scanf("%s", find_name);
      for (int i = 0; i < size; ++i) {
        if (!strcmp(find_name, guide[i].name)) {
          printf("Запись номер %d\nИмя абонента - %s\nФамилия абонента - "
                 "%s\nТелефон абонента - %s\n",
                 i + 1, guide[i].name, guide[i].second_name, guide[i].tel);
        }
      }
      break;
    case 4:
      for (int i = 0; i < size; ++i) {
        printf("Запись номер %d\nИмя абонента - %s\nФамилия абонента - "
               "%s\nТелефон абонента - %s\n",
               i + 1, guide[i].name, guide[i].second_name, guide[i].tel);
      }
      break;
    case 5:
      stop = 0;
      break;
    default:
      printf("Некорректный ввод\n");
      break;
    }
  }
}