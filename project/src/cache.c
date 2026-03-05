#include "cache.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "lru.h"

char *make_block(int block_size) {
  // TODO:
  //   Make and initialize a block of memory given the block_size.
  //   Note that the memory should be initialized with zeros.
  //
  return NULL;
}

Line *make_lines(int line_count, int block_size) {
  // TODO:
  //   Make and initialize the lines given the line count.
  //   Then make and initialize the blocks using make_block function.
  //
  return NULL;
}

Set *make_sets(int set_count, int line_count, int block_size) {
  // TODO:
  //   Make and initialize the sets given the set count. Then
  //   make and initialize the lines and blocks.
  //
  return NULL;
}

Cache *make_cache(int set_bits, int line_count, int block_bits) {
  Cache *cache = NULL;
  // TODO:
  //   Make and initialize the cache, sets, lines, and blocks.
  //   You should use the `exp2` function to determine the
  //   set_count and block_count from the set_bits and block_bits
  //   respectively (use `man exp2` from the command line).
  //
  // ADD YOUR CODE HERE:

  // END TODO

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
