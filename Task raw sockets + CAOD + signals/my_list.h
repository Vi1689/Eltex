#ifndef MY_LIST__
#define MY_LIST__

#include <stdint.h>
#include <unistd.h>

struct client {
  uint16_t sin_port;
  uint32_t sin_addr;
  size_t count;
};

struct node {
  struct node *next;
  struct client data;
};

struct list {
  struct node *head;
};

struct list *create();

struct node *create_node(uint16_t sin_port, uint32_t sin_addr);

size_t push(struct list *list, uint32_t sin_addr, uint16_t sin_port);

void pop(struct list *list, uint32_t sin_addr, uint16_t sin_port);

void clear_list(struct list *list);

void delete(struct list *list);

#endif