#include "list.h"

#include <malloc.h>
#include <string.h>

struct list *create() {

  struct list *tmp = malloc(sizeof(struct list));
  tmp->head = NULL;
  tmp->size = 0;
  return tmp;
}

struct node *create_node(int value, char name[]) {

  struct node *tmp = malloc(sizeof(struct node));
  tmp->value = value;
  memset(tmp->name, '\000', sizeof(tmp->name));
  memmove(tmp->name, name, strlen(name));
  tmp->next = NULL;
  return tmp;
}

int lookup(struct list *list, char name[]) {

  struct node *tmp = list->head;
  while (tmp) {
    if (strcmp(tmp->name, name) == 0) {
      return tmp->value;
    }
    tmp = tmp->next;
  }

  return -1;
}

char *get_name(struct list *list, int value) {

  struct node *tmp = list->head;
  while (tmp) {
    if (value == tmp->value) {
      return tmp->name;
    }
    tmp = tmp->next;
  }

  return NULL;
}

void push(struct list *list, int value, char name[]) {

  struct node *tmp = list->head;
  if (tmp == NULL) {
    list->head = create_node(value, name);
    list->size++;
    return;
  }
  while (tmp->next != NULL) {
    tmp = tmp->next;
  }
  tmp->next = create_node(value, name);
  list->size++;
}

void pop(struct list *list, int value) {

  struct node *tmp = list->head, *parent = NULL;
  if (tmp == NULL) {
    return;
  }
  while (tmp) {
    if (tmp->value == value) {
      list->size--;
      if (parent)
        parent->next = tmp->next;
      if (!parent)
        list->head = tmp->next;

      free(tmp);

      break;
    }
    parent = tmp;
    tmp = tmp->next;
  }
}

void clear_list(struct list *list) {

  struct node *tmp = list->head;

  while (tmp) {
    struct node *a = tmp;
    tmp = tmp->next;
    free(a);
    list->size--;
  }
  list->head = NULL;
}

void delete(struct list *list) {

  struct node *tmp = list->head;

  while (tmp) {
    struct node *a = tmp;
    tmp = tmp->next;
    free(a);
  }
  free(list);
}