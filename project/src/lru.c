#include "lru.h"
#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

void lru_init_queue(Set *set) {
  LRUNode *s = NULL;
  LRUNode **pp = &s;  // place to chain in the next node
  for (int i = 0; i < set->line_count; i++) {
    Line *line = &set->lines[i];
    LRUNode *node = (LRUNode *)(malloc(sizeof(LRUNode)));
    node->line = line;
    node->next = NULL;
    (*pp) = node;
    pp = &((*pp)->next);
  }
  set->lru_queue = s;
}

void lru_init(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    lru_init_queue(&sets[i]);
  }
}

void lru_destroy(Cache *cache) {
  Set *sets = cache->sets;
  for (int i = 0; i < cache->set_count; i++) {
    LRUNode *p = sets[i].lru_queue;
    LRUNode *n = p;
    while (p != NULL) {
      p = p->next;
      free(n);
      n = p;
    }
    sets[i].lru_queue = NULL;
  }
}

void lru_fetch(Set *set, unsigned int tag, LRUResult *result) {
  LRUNode *curr = set->lru_queue;
  LRUNode *prev = NULL;
  LRUNode *target_node = NULL;
  LRUNode *target_prev = NULL;

  
  while (curr != NULL) {
    if (curr->line->valid && curr->line->tag == tag) {
      //  HIT
      result->access = HIT;
      target_node = curr;
      target_prev = prev;
      break;
    } else if (!curr->line->valid && target_node == NULL) {
      // COLD_MISS 
      result->access = COLD_MISS;
      target_node = curr;
      target_prev = prev;
    }
    prev = curr;
    curr = curr->next;
  }

  // CONFLICT_MISS 
  if (result->access != HIT && target_node == NULL) {
    result->access = CONFLICT_MISS;
    target_node = prev;      
    target_prev = NULL;       
    LRUNode *p = set->lru_queue;
    target_prev = NULL;
    while (p->next != target_node) {
      target_prev = p;
      p = p->next;
    }
    target_prev = p;
  }

  
  target_node->line->tag = tag;
  target_node->line->valid = 1;
  result->line = target_node->line;
  if (target_prev != NULL) {
    target_prev->next = target_node->next;
    target_node->next = set->lru_queue;
    set->lru_queue = target_node;
  }
}
