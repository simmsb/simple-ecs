#include <stdint.h>

#include "entity.h"

uint32_t new_entity_id(void) {
  static uint32_t id_counter = 0;

  return id_counter++;
}
