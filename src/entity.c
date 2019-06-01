#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "entity.h"

static uint32_t id_counter = 0;

uint32_t new_entity_id(void) {
  if (id_counter == UINT32_MAX) {
    fprintf(stderr, "out of entities");
    exit(1);
  }

  return id_counter++;
}

void reset_ent_counter(void) { id_counter = 0; }

void kill_entity(uint32_t id) {
  for (struct component_def **s = ({
         extern struct component_def *__start_component_def_array;
         &__start_component_def_array;
       });
       s != ({
         extern struct component_def *__stop_component_def_array;
         &__stop_component_def_array;
       });
       s++) {
    (*s)->delete_value(id);
  }
}

void remove_all_entities(void) {
  for (struct component_def **s = ({
         extern struct component_def *__start_component_def_array;
         &__start_component_def_array;
       });
       s != ({
         extern struct component_def *__stop_component_def_array;
         &__stop_component_def_array;
       });
       s++) {
    (*s)->clear_everything();
  }
}
