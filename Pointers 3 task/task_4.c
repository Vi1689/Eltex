#include <stdio.h>

size_t length(char *str) {
  size_t size = 0;
  while (str[size] != '\000') {
    size++;
  }
  return size;
}

char *find(char *string, char *substring) {
  size_t string_size = length(string), substring_size = length(substring);

  for (size_t i = 0; i < string_size - substring_size; ++i) {
    size_t count = 0;
    for (size_t j = 0; j < substring_size; ++j) {
      count += (string[i + j] == substring[j]);
    }
    if (count == substring_size) {
      return string + i;
    }
  }

  return NULL;
}

int main() {
  char string[100];
  char subctring[20];

  printf("Введите строку\n");
  scanf("%s", string);

  printf("Введите подстроку\n");
  scanf("%s", subctring);

  char *temp = find(string, subctring);
  temp ? printf("%s\n", temp) : printf("Подстрока не найдена\n");

  return 0;
}