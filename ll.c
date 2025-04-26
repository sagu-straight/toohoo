#include "ll.h"
#include "entity.h"
#include <stdlib.h>

void add_node_to_list(list* l, entity* e) {
  node* n = malloc(sizeof(node));
  n->e = e;

  if (l->start == NULL){
    n->next = NULL;
    n->prev = NULL;
    l->start = n;
    l->end = n;
    return;
  }

  n->prev = l->end;
  n->next = NULL;
  l->end->next = n;
  l->end = n;
}

void remove_node_from_list(list* l, node* n) {
  if (l->start == n) {
    l->start = n->next;
    l->start->prev = NULL;
    return;
  }
  if (l->end == n) {
    l->end = n->prev;
    l->end->next = NULL;
    return;
  }

  n->prev->next = n->next;
  n->next->prev = n->prev;
}

void initialize_list(list* l) {
  l->start = NULL;
  l->end = NULL;
}

