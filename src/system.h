#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include <stdint.h>
#include <string.h>

// Systems of the entity component system

// TODO:
// - macros for joining components
// - more magick

/**
 * Register a system, usage:
 *
 * REGISTER_SYSTEM(name, {
 *     printf("stuff\n");
 * });
 *
 * Systems should be registered in an order they will be executed in.
 */
#define REGISTER_SYSTEM(NAME, ...)                                             \
  static void system_callback__##NAME(void) { __VA_ARGS__ }                    \
  static struct system_def NAME = {.name = #NAME, .id = __COUNTER__, .cb = &system_callback__##NAME};                                                                       \
  static struct system_def *system_ptr__##NAME __attribute__((used, section("system_def_array"))) = &NAME;                      \
  /* static const uint32_t system_##NAME##_id = __COUNTER__;                      \ */
  /* static void system_init__##NAME(void) __attribute__((constructor));          \ */
  /* static void system_init__##NAME(void) {                                      \ */
  /*   memcpy(&NAME,                                                              \ */
  /*          &(struct system_def){.name = #NAME,                                 \ */
  /*                               .id = system_##NAME##_id,                      \ */
  /*                               .cb = &system_callback__##NAME},               \ */
  /*          sizeof(struct system_def));                                         \ */
  /* } */

struct system_def {
  const char *const name;
  const uint32_t id;
  void (*const cb)(void);
};

/**
 * Run all systems in the program.
 */
void run_systems(void);

#endif // __SYSTEM_H_
