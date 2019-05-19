#include <stdio.h>

#include "system.h"

void run_systems(void) {
  for (struct system_def **s = ({
         extern struct system_def *__start_system_def_array;
         &__start_system_def_array;
       });
       s != ({
         extern struct system_def *__stop_system_def_array;
         &__stop_system_def_array;
       });
       s++) {
    (*s)->cb();
  }
}
