// basic linked list for storing entities
#ifndef __LL__
#define __LL__

#include "entity.h"

typedef struct node{
  struct node* next;
  struct node* prev;
  entity* e;
} node;

typedef struct {
  node* start;
  node* end;
} list;

void add_node_to_list(list* l, entity* e);
void remove_node_from_list(list* l, node* n);
void initialize_list(list* l);

#endif
