#include <stdio.h>
#include <string.h>

int main() {
  FILE *in = fopen("output.txt", "w");
  if (!in) {
    perror("The file is not open\n");
    return 1;
  }

  char str[] = "String from file";

  fwrite(str, sizeof(str[0]), strlen(str), in);

  fclose(in);
  FILE *out = fopen("output.txt", "r");
  if (!out) {
    perror("The file is not open\n");
    return 1;
  }

  fseek(out, 0, SEEK_END);

  char str_2[17];

  for (int i = 1; i < 17; ++i) {
    fseek(out, -i, SEEK_END);
    fscanf(out, "%c", &str_2[i - 1]);
  }
  str_2[16] = '\000';
  printf("%s\n", str_2);

  fclose(out);
  return 0;
}