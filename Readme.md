 # Simple ECS

 This is a simple ECS framework for C

# Example

```c
#include <stdio.h>
#include <stdlib.h>

#include "entity.h"
#include "component.h"
#include "system.h"

struct position_storage {
  int32_t x, y;
};

DEFINE_COMPONENT(position, struct position_storage);
REGISTER_COMPONENT(position, struct position_storage);

struct velocity_storage {
  int32_t dx, dy;
};

DEFINE_COMPONENT(velocity, struct velocity_storage);
REGISTER_COMPONENT(velocity, struct velocity_storage);

REGISTER_SYSTEM(update_velocty_values, {
  FOR_JOIN_COMPONENT_2(position, velocity, d, {
    d.position->x += d.velocity->dx;
    d.position->y += d.velocity->dy;
  });
});

int main() {
  uint32_t test_entity = new_entity_id();

  position.add_value(test_entity, (struct position_storage){.x = 0, .y = 0});
  velocity.add_value(test_entity, (struct velocity_storage){.dx = 1, .dy = 2});

  while (true) {
    run_systems();
  }
}
```
