#ifndef __COMPONENT_H_
#define __COMPONENT_H_

#include <stdint.h>
#include <string.h>

#include "hash_set.h"
#include "hash_table.h"

#define STRUCT_MEMBER_TYPE(TYPE, MEMBER) typeof(((TYPE *)0)->MEMBER)

// Components of the entity component system

#define COMPONENT_DEF(NAME, TYPE)                                              \
  struct component_##NAME##_def {                                              \
    const char *const name;                                                    \
    const uint32_t id;                                                         \
    struct hash_table_component_##NAME##_storage *const storage;               \
    void (*const add_value)(uint32_t ent_id, TYPE val);                        \
    TYPE *(*const lookup_value)(uint32_t ent_id);                              \
    void (*const delete_value)(uint32_t ent_id);                               \
  };

#define DEFINE_COMPONENT(NAME, TYPE)                                           \
  DEFINE_HASH(TYPE, component_##NAME##_storage);                               \
  COMPONENT_DEF(NAME, TYPE);

#define REGISTER_COMPONENT(NAME, TYPE)                                         \
  MAKE_HASH(TYPE, component_##NAME##_storage);                                 \
  static struct component_##NAME##_def NAME                                    \
      __attribute__((used, section("component_def_array")));                   \
  static const uint32_t component_##NAME##_id = __COUNTER__;                   \
  void component_##NAME##_add_value(uint32_t ent_id, TYPE val) {               \
    hash_table_component_##NAME##_storage_insert(NAME.storage, ent_id, val);   \
  }                                                                            \
  TYPE *component_##NAME##_lookup_value(uint32_t ent_id) {                     \
    return hash_table_component_##NAME##_storage_lookup(NAME.storage, ent_id); \
  }                                                                            \
  void component_##NAME##_delete_value(uint32_t ent_id) {                      \
    hash_table_component_##NAME##_storage_delete(NAME.storage, ent_id);        \
  }                                                                            \
  static void component_init__##NAME(void) __attribute__((constructor));       \
  static void component_init__##NAME(void) {                                   \
    memcpy(&NAME,                                                              \
           &(struct component_##NAME##_def){                                   \
               .name = #NAME,                                                  \
               .id = component_##NAME##_id,                                    \
               .storage = hash_table_component_##NAME##_storage_new(),         \
               .add_value = &component_##NAME##_add_value,                     \
               .lookup_value = &component_##NAME##_lookup_value,               \
               .delete_value = &component_##NAME##_delete_value},              \
           sizeof(struct component_##NAME##_def));                             \
  }

/**
 * Union of all entities that have the given components.
 *
 * Used to loop over all entites and components.
 * @param COMP_NAME component to iterate over.
 * @param ITER_VAR variable to receive each value of the iteration
 *        will be given the type of `struct {uint32_t id; COMP_TYPE
 * *COMP_NAME;}` where `COMP_TYPE` is the storage type of the component
 * `COMP_NAME`.
 *
 * Usage:
 * FOR_JOIN_COMPONENT_1(my_component, i, {
 *    printf("entity id: %u, component_val: %d\n", i.id,
 * i.my_component->whatever);
 * });
 */
#define FOR_JOIN_COMPONENT_1(COMP_NAME, ITER_VAR, ...)                         \
  do {                                                                         \
    HASH_TABLE_ITER(component_##COMP_NAME##_storage, k, v, COMP_NAME.storage,  \
                    {                                                          \
                      struct {                                                 \
                        uint32_t id;                                           \
                        typeof(v) COMP_NAME;                                   \
                      } ITER_VAR = {k, v};                                     \
                      { __VA_ARGS__ }                                          \
                    });                                                        \
  } while (0)

/**
 * Union of all entities that have the given components.
 *
 * Used to loop over all entites and components.
 * @param COMP_NAME_0, COMP_NAME_1 components to iterate over.
 * @param ITER_VAR variable to receive each value of the iteration
 *        will be given the type of `struct {uint32_t id; COMP_TYPE_0
 * *COMP_NAME_0, COMP_TYPE_1, *COMP_NAME_1;}` where `COMP_TYPE_x` is the storage
 * type of the component `COMP_NAME_x`.
 *
 * Usage:
 * FOR_JOIN_COMPONENT_1(my_component, my_other_component, i, {
 *    printf("entity id: %u, component_val: %d, my_other_component_val: %d\n",
 * i.id, i.my_component->whatever, i.my_other_component->something);
 * });
 */
#define FOR_JOIN_COMPONENT_2(COMP_NAME_0, COMP_NAME_1, ITER_VAR, ...)          \
  do {                                                                         \
    HASH_TABLE_ITER(                                                           \
        component_##COMP_NAME_0##_storage, k_0, v_0, COMP_NAME_0.storage, {    \
          STRUCT_MEMBER_TYPE(                                                  \
              struct hash_table_component_##COMP_NAME_1##_storage_elem,        \
              val) *v_1 =                                                      \
              hash_table_component_##COMP_NAME_1##_storage_lookup(             \
                  COMP_NAME_1.storage, k_0);                                   \
          if (v_1 != NULL) {                                                   \
            struct {                                                           \
              uint32_t id;                                                     \
              typeof(v_0) COMP_NAME_0;                                         \
              typeof(v_1) COMP_NAME_1;                                         \
            } ITER_VAR = {k_0, v_0, v_1};                                      \
            { __VA_ARGS__ }                                                    \
          }                                                                    \
        });                                                                    \
  } while (0)

/**
 * Union of all entities that have the given components.
 *
 * Used to loop over all entites and components.
 * @param COMP_NAME_0, COMP_NAME_1, COMP_NAME_2 components to iterate over.
 * @param ITER_VAR variable to receive each value of the iteration will be given
 * the type of `struct {uint32_t id; COMP_TYPE_0 *COMP_NAME_0, COMP_TYPE_1,
 * *COMP_NAME_1; COMP_TYPE_2, *COMP_NAME_2}` where `COMP_TYPE_x` is the storage
 * type of the component `COMP_NAME_x`.
 *
 * Usage:
 * FOR_JOIN_COMPONENT_1(my_component, my_other_component, another_component, i,
 * { printf("entity id: %u, component_val: %d, my_other_component_val: %d,
 * another_component_val: %d\n", i.id, i.my_component->whatever,
 * i.my_other_component->something, i.another_component->it);
 * });
 */
#define FOR_JOIN_COMPONENT_3(COMP_NAME_0, COMP_NAME_1, ITER_VAR, ...)          \
  do {                                                                         \
    HASH_TABLE_ITER(                                                           \
        component_##COMP_NAME_0##_storage, k_0, v_0, COMP_NAME_0.storage, {    \
          STRUCT_MEMBER_TYPE(                                                  \
              struct hash_table_component_##COMP_NAME_1##_storage_elem,        \
              val) *v_1 =                                                      \
              hash_table_component_##COMP_NAME_1##_storage_lookup(             \
                  COMP_NAME_1.storage, k_0);                                   \
          STRUCT_MEMBER_TYPE(                                                  \
              struct hash_table_component_##COMP_NAME_2##_storage_elem,        \
              val) *v_2 =                                                      \
              hash_table_component_##COMP_NAME_2##_storage_lookup(             \
                  COMP_NAME_1.storage, k_0);                                   \
          if (v_1 != NULL && v_2 != NULL) {                                    \
            struct {                                                           \
              uint32_t id;                                                     \
              typeof(v_0) COMP_NAME_0;                                         \
              typeof(v_1) COMP_NAME_1;                                         \
              typeof(v_2) COMP_NAME_2;                                         \
            } ITER_VAR = {k_0, v_0, v_1, v_2};                                 \
            { __VA_ARGS__ }                                                    \
          }                                                                    \
        });                                                                    \
  } while (0)

#endif // __COMPONENT_H_
