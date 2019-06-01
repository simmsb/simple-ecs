#ifndef __ENTITY_H_
#define __ENTITY_H_

#include <stdint.h>

/**
 * Get a new entity id.
 */
uint32_t new_entity_id(void);
void reset_ent_counter(void);
void kill_entity(uint32_t id);
void remove_all_entities(void);

#endif // __ENTITY_H_
