#include <stdio.h>
#include <stdlib.h>

void print_matrix(int *ans, int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      printf("%d ", ans[i * n + j]);
    }
    printf("\n");
  }
}

void task_1(int n) {
  int *ans = malloc(sizeof(int) * n * n);
  for (int i = 0; i < n * n; ++i) {
    ans[i] = i + 1;
  }
  print_matrix(ans, n);
  free(ans);
}

void task_2(int n) {
  int *ans = malloc(sizeof(int) * n);
  for (int i = 0; i < n; ++i) {
    ans[i] = i + 1;
  }
  for (int i = 0; i < n; ++i) {
    printf("%d ", ans[i]);
  }
  printf("\n");
  int size = n / 2 + (n & 1);
  for (int i = 0; i < size; ++i) {
    int temp = ans[i];
    ans[i] = ans[n - 1 - i];
    ans[n - 1 - i] = temp;
  }
  for (int i = 0; i < n; ++i) {
    printf("%d ", ans[i]);
  }
  printf("\n");
  free(ans);
}

void task_3(int n) {
  int *ans = malloc(sizeof(int) * n * n);
  int k = n - 1;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      ans[i * n + j] = (k <= j);
    }
    k--;
  }
  print_matrix(ans, n);
  free(ans);
}

void task_4(int n) {
  int *ans = malloc(sizeof(int) * n * n);
  int size = n / 2 + (n & 1), count = 1, k = 0, a = 0, b = 0;
  for (int j = 0; j < size; ++j) {
    a = j, b = j;
    for (int i = 0; i < n - k; ++i) {
      ans[a * n + (b + i)] = count++;
    }
    k++;
    a = j + 1, b = n - 1 - j;
    for (int i = 0; i < n - k; ++i) {
      ans[(a + i) * n + b] = count++;
    }
    a = n - 1 - j, b = n - 2 - j;
    for (int i = 0; i < n - k; ++i) {
      ans[a * n + (b - i)] = count++;
    }
    k++;
    a = n - 2 - j, b = j;
    for (int i = 0; i < n - k; ++i) {
      ans[(a - i) * n + b] = count++;
    }
  }
  print_matrix(ans, n);
  free(ans);
}

int main() {
  int n;
  scanf("%d", &n);
  printf("Первое задание\n");
  task_1(n);
  printf("Второе задание\n");
  task_2(n);
  printf("Третье задание\n");
  task_3(n);
  printf("Четвёртое задание\n");
  task_4(n);
  return 0;
}