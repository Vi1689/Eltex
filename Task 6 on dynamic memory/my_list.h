#ifndef _MY_LIST_
#define _MY_LIST_

struct abonent {
  char name[10];
  char second_name[10];
  char tel[10];
};

struct list {
  struct abonent data;
  struct list *prev;
  struct list *next;
};

struct root {
  struct list *value;
};

struct root *create();
struct list *node_create(struct abonent a);
void add(struct root *guid, struct abonent a);
void delete(struct root *guid, struct abonent a);
void find(struct root *guid, char *name);
void print(struct root *guid);
void free_node(struct list *temp);
void free_list(struct root *guid);

#endif