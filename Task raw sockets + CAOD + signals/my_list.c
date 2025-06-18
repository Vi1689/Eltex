#include "my_list.h"
#include <malloc.h>
#include <string.h>

struct list *create() {

  struct list *tmp = malloc(sizeof(struct list));
  tmp->head = NULL;
  return tmp;
}

struct node *create_node(uint16_t sin_port, uint32_t sin_addr) {

  struct node *tmp = malloc(sizeof(struct node));
  tmp->data.count = 0;
  tmp->data.sin_port = sin_port;
  tmp->data.sin_addr = sin_addr;
  tmp->next = NULL;
  return tmp;
}

size_t push(struct list *list, uint32_t sin_addr, uint16_t sin_port) {

  struct node *tmp = list->head, *parent = NULL;
  if (tmp == NULL) {
    list->head = create_node(sin_port, sin_addr);
    return ++list->head->data.count;
  }

  while (tmp) {
    if (sin_addr == tmp->data.sin_addr && sin_port == tmp->data.sin_port) {
      return ++tmp->data.count;
    }
    parent = tmp;
    tmp = tmp->next;
  }
  parent->next = create_node(sin_port, sin_addr);
  return ++parent->next->data.count;
}

void pop(struct list *list, uint32_t sin_addr, uint16_t sin_port) {

  struct node *tmp = list->head, *parent = NULL;
  if (tmp == NULL) {
    return;
  }
  while (tmp) {
    if (sin_addr == tmp->data.sin_addr && sin_port == tmp->data.sin_port) {

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