#ifndef MY_LIST__
#define MY_LIST__

struct list {
  int size;
  struct node *head;
};

struct node {
  int value;
  char name[1024];
  struct node *next;
};

struct list *create();

struct node *create_node(int value, char name[]);

int lookup(struct list *list, char name[]);

char *get_name(struct list *list, int value);

void push(struct list *list, int value, char name[]);

void pop(struct list *list, char name[]);

void clear_list(struct list *list);

void delete(struct list *list);

#endif