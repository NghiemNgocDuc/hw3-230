#include "cache.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "lru.h"

char *make_block(int block_size) {
  char *block = calloc(block_size, sizeof(char));
  return block;
}

Line *make_lines(int line_count, int block_size) {
  Line *duc = calloc(line_count, sizeof(Line));
  for (int i =0; i< line_count; i++){
    duc[i].block = make_block(block_size);
    duc[i].tag = 0;
    duc[i].valid =0;
  }
  return duc;
}

Set *make_sets(int set_count, int line_count, int block_size) {
  Set *hanh = calloc(set_count, sizeof(Set));
  for (int i=0; i<set_count; i++){
    hanh[i].lines = make_lines(line_count, block_size);
    hanh[i].line_count = line_count;
  }
  return hanh;
}

Cache *make_cache(int set_bits, int line_count, int block_bits) {
  Cache *cache = malloc(sizeof(Cache));
  cache->set_bits = set_bits;
  cache->block_bits = block_bits;
  cache->set_count = (int)exp2(set_bits);
  cache->block_size = (int)exp2(block_bits);
  cache->line_count = line_count;
  cache->sets = make_sets(cache->set_count, line_count, cache->block_size);
  return cache;
  // Create LRU queues for sets:
  if (cache != NULL) {
    lru_init(cache);
  }

  return cache;
}

void delete_block(char *accessed) { free(accessed); }

void delete_lines(Line *lines, int line_count) {
  for (int i = 0; i < line_count; i++) {
    delete_block(lines[i].block);
  }
  free(lines);
}

void delete_sets(Set *sets, int set_count) {
  for (int i = 0; i < set_count; i++) {
    delete_lines(sets[i].lines, sets[i].line_count);
  }
  free(sets);
}

void delete_cache(Cache *cache) {
  lru_destroy(cache);
  delete_sets(cache->sets, cache->set_count);
  free(cache);
}

SearchInfo get_bits(Cache *cache, address_type address) {
  SearchInfo result;

  // TODO:
  //  Extract the set bits, tag bits, and block bits from a 32-bit address into
  //    result.
  //
  return result;
}

AccessResult cache_access(Cache *cache, TraceLine *trace_line) {
  SearchInfo bits = get_bits(cache, trace_line->address);
  unsigned int s = bits.set_id;
  unsigned int t = bits.tag;
  unsigned int b = bits.offset;

  // Get the set:
  Set *set = &cache->sets[s];

  // Get the line:
  LRUResult result;
  lru_fetch(set, t, &result);
  Line *line = result.line;

  // If it was a miss we will clear the accessed bits:
  if (result.access != HIT) {
    for (int i = 0; i < cache->block_size; i++) {
      line->block[i] = 0;
    }
  }

  // Then set the accessed byte to 1:
  line->block[b] = 1;

  return result.access;
}
