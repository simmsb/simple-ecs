#include <stdbool.h>
#include <stdint.h>

#include "bit_array.h"
#include "hash_set.h"
#include "common_macros.h"

bool hash_set_is_entry_deleted(struct hash_set *table, uint32_t idx) {
  return get_bit_in_bitarray(table->deleted, idx);
}

static void hash_set__mark_deleted(struct hash_set *table, uint32_t idx) {
  set_bit_in_bitarray(table->deleted, idx, true);
}

static void hash_set__reset_deleted(struct hash_set *table, uint32_t idx) {
  set_bit_in_bitarray(table->deleted, idx, false);
}

uint32_t hash_set_hash_fun(uint32_t k) {
  const uint32_t hash_constant = 0x45d9f3b;

  k = ((k >> 16) ^ k) * hash_constant;
  k = ((k >> 16) ^ k) * hash_constant;
  k = (k >> 16) ^ k;

  return k * hash_constant;
}

uint32_t hash_set__fix_hash(uint32_t h) {
  if (h) {
    return h;
  }

  return 1;
}

uint32_t hash_set_hash_idx(struct hash_set *table, uint32_t hash) {
  return hash & table->mask;
}

uint32_t hash_set_max_probes(struct hash_set *table, uint32_t hash,
                             uint32_t idx) {
  return (table->cap + idx - hash_set_hash_idx(table, hash)) & table->mask;
}

static void hash_set__set_elem(struct hash_set *table, uint32_t idx, uint32_t h,
                               uint32_t k) {
  table->elems[idx] = (struct hash_set_elem){h, k};
}

static void hash_set__insert(struct hash_set *table, struct hash_set_elem e) {
  uint32_t idx = hash_set_hash_idx(table, e.hash);
  uint32_t to_insert_elem_probes = 0;

  for (;;) {
    // fast case, element where we want to insert is empty
    if (!table->elems[idx].hash) {
      table->elems[idx] = e;

      return;
    }

    uint32_t current_elem_probes =
        hash_set_max_probes(table, table->elems[idx].hash, idx);

    // if we're here, the element was occupied or deleted
    // steal from the rich, give to the poor
    if (current_elem_probes < to_insert_elem_probes) {

      // the element is deleted, just replace it
      if (hash_set_is_entry_deleted(table, idx)) {

        // undelete
        hash_set__reset_deleted(table, idx);

        table->elems[idx] = e;

        return;
      }

      // element wasn't deleted, swap element to insert with it and continue
      SWAP(e, table->elems[idx]);
      to_insert_elem_probes = current_elem_probes;
    }

    idx++;
    idx &= table->mask;
    to_insert_elem_probes++;

    /* printf("idx: %d, number probes: %d\n", idx, to_insert_elem_probes); */
  }
}

static int64_t hash_set__lookup(struct hash_set *table, uint32_t k) {
  uint32_t hash = hash_set__fix_hash(hash_set_hash_fun(k));
  uint32_t idx = hash_set_hash_idx(table, hash);

  uint32_t num_probes = 0;

  for (;;) {
    uint32_t current_hash = table->elems[idx].hash;

    // if the entry is empty and not deleted, nothing is here
    if (!current_hash) {
      return -1;
    }

    // if we've proved enough times to check every possible entry, nothing is
    // here
    if (num_probes > hash_set_max_probes(table, current_hash, idx)) {
      return -1;
    }

    // current element isn't deleted, and both the hash and keys match
    if (!hash_set_is_entry_deleted(table, idx) && current_hash == hash &&
        table->elems[idx].key == k) {
      return idx;
    }

    idx++;
    idx &= table->mask;
    num_probes++;
  }
}

static void hash_set__construct(struct hash_set *table,
                                uint32_t initial_capacity) {
  table->elems = calloc(initial_capacity, sizeof(struct hash_set_elem));
  table->deleted = bit_array_new(initial_capacity);
  table->num_elems = 0;
  table->cap = initial_capacity;
  table->mask = initial_capacity - 1;
  table->resize_thresh =
      (initial_capacity * hash_set_load_factor_to_grow) / 100;
}

struct hash_set *hash_set_new() {
  struct hash_set *table = malloc(sizeof(struct hash_set));
  hash_set__construct(table, hash_set_initial_cap);
  return table;
}

void hash_set_free(struct hash_set *table) {
  free(table->elems);
  free(table->deleted);
}

void hash_set_grow(struct hash_set *table) {
  struct hash_set new_table;
  hash_set__construct(&new_table, table->cap * 2);

  new_table.num_elems = table->num_elems;

  for (uint32_t i = 0; i < table->cap; i++) {
    struct hash_set_elem e = table->elems[i];

    if (e.hash && !hash_set_is_entry_deleted(table, i)) {
      hash_set__insert(&new_table, e);
    }
  }

  hash_set_free(table);
  *table = new_table;
}

void hash_set_insert(struct hash_set *table, uint32_t k) {
  uint32_t hash = hash_set__fix_hash(hash_set_hash_fun(k));

  table->num_elems++;

  if (table->num_elems >= table->resize_thresh) {
    /* printf("growing table\n"); */
    hash_set_grow(table);
  }

  hash_set__insert(table, (struct hash_set_elem){hash, k});
}

bool hash_set_contains(struct hash_set *table, uint32_t k) {
  int64_t idx = hash_set__lookup(table, k);

  return !(idx < 0);
}

bool hash_set_delete(struct hash_set *table, uint32_t k) {
  uint32_t hash = hash_set__fix_hash(hash_set_hash_fun(k));
  int64_t idx = hash_set__lookup(table, k);

  if (idx < 0) {
    return false;
  }

  hash_set__mark_deleted(table, idx);
  table->num_elems--;
  return true;
}
