#ifndef __HASH_SET_H_
#define __HASH_SET_H_

// A hash set implementation using robin hood hashing

#include <stdbool.h>
#include <stdint.h>

#include "bit_array.h"
#include "common_macros.h"

static const uint32_t hash_set_initial_cap = 256;
static const uint8_t hash_set_load_factor_to_grow = 90;

struct hash_set_elem {
  uint32_t hash;
  uint32_t key;
};

struct hash_set {
  struct hash_set_elem *elems;
  uint8_t *deleted;
  uint32_t num_elems;
  uint32_t cap;
  uint32_t mask;
  uint resize_thresh;
};

bool hash_set_is_entry_deleted(struct hash_set *table, uint32_t idx);

uint32_t hash_set_hash_fun(uint32_t k);

uint32_t hash_set_hash_idx(struct hash_set *table, uint32_t hash);

uint32_t hash_set_max_probes(struct hash_set *table, uint32_t hash,
                             uint32_t idx);

struct hash_set *hash_set_new();

void hash_set_free(struct hash_set *table);

void hash_set_grow(struct hash_set *table);

void hash_set_insert(struct hash_set *table, uint32_t k);
bool hash_set_contains(struct hash_set *table, uint32_t k);

bool hash_set_delete(struct hash_set *table, uint32_t k);

#define HASH_SET_ITER(ELEM_NAME, TABLE, ...)                                   \
  for (uint32_t hash_set_iter_idx = 0; hash_set_iter_idx < (TABLE)->cap;       \
       hash_set_iter_idx++) {                                                  \
    struct hash_set_##NAME##_elem hash_set_iter_e =                            \
        (TABLE)->elems[hash_set_iter_idx];                                     \
    if (hash_set_iter_e.hash &&                                                \
        !hash_set_##NAME##__is_entry_deleted((TABLE), hash_set_iter_idx)) {    \
      uint32_t ELEM_NAME = hash_set_iter_e.key;                                \
      { __VA_ARGS__ }                                                          \
    }                                                                          \
  }

#endif // __HASH_SET_H_
