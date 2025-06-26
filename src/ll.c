#include "../headers/ll.h"
#include <stdlib.h>

void add_node_to_list(list* l, void* e) {
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
  if (!l->start || !n) return;
  
  if (l->start == n) {
    l->start = n->next;

    if (l->start)
      l->start->prev = NULL;
    else
      l->end = NULL;

    return;
  }
  if (l->end == n) {
    l->end = n->prev;

    if (l->end)
      l->end->next = NULL;
    else
      l->start = NULL;

    return;
  }

  n->prev->next = n->next;
  n->next->prev = n->prev;
}

void initialize_list(list* l) {
  l->start = NULL;
  l->end = NULL;
}

