#include "my_list.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct root *create() {
  struct root *node = malloc(sizeof(*node));
  if (node) {
    node->value = NULL;
  }
  return node;
}

struct list *node_create(struct abonent a) {
  struct list *node = malloc(sizeof(*node));
  if (node) {
    node->data = a;
    node->next = NULL;
    node->prev = NULL;
  }
  return node;
}

void add(struct root *guid, struct abonent a) {
  if (!guid) {
    return;
  }
  if (!guid->value) {
    guid->value = node_create(a);
    return;
  }
  struct list *node = node_create(a);
  struct list *temp = guid->value, *parent;
  while (temp != NULL) {
    parent = temp;
    temp = temp->next;
  }
  parent->next = node;
  node->prev = parent;
}

void delete(struct root *guid, struct abonent a) {
  if (!guid) {
    return;
  }
  struct list *temp = guid->value, *parent;
  while (temp != NULL) {
    parent = temp;
    temp = temp->next;
    if (!strcmp(parent->data.name, a.name) &&
        !strcmp(parent->data.second_name, a.second_name) &&
        !strcmp(parent->data.tel, a.tel)) {
      if (guid->value == parent) {
        guid->value = parent->next;
      }
      if (parent->next) {
        parent->next->prev = parent->prev;
      }
      if (parent->prev) {
        parent->prev->next = parent->next;
      }
      free(parent);
      break;
    }
  }
}

void find(struct root *guid, char *name) {
  if (!guid) {
    return;
  }
  struct list *temp = guid->value;
  int size = 0, found = 0;
  while (temp != NULL) {
    if (!strcmp(name, temp->data.name)) {
      printf("Запись номер %d\nИмя абонента - %s\nФамилия абонента - "
             "%s\nТелефон абонента - %s\n",
             size + 1, temp->data.name, temp->data.second_name, temp->data.tel);
      found = 1;
    }
    temp = temp->next;
    size++;
  }
  if (!found) {
    printf("Нечего не найдено\n");
  }
}

void print(struct root *guid) {
  if (!guid) {
    return;
  }
  struct list *temp = guid->value;
  int size = 0;
  while (temp != NULL) {
    printf("Запись номер %d\nИмя абонента - %s\nФамилия абонента - "
           "%s\nТелефон абонента - %s\n",
           size + 1, temp->data.name, temp->data.second_name, temp->data.tel);
    temp = temp->next;
    size++;
  }
  if (!size) {
    printf("Справочник пуст\n");
  }
}

void free_node(struct list *temp) {
  if (!temp) {
    return;
  }
  free_node(temp->next);
  free(temp);
}

void free_list(struct root *guid) {
  if (!guid) {
    return;
  }
  free_node(guid->value);
  free(guid);
}