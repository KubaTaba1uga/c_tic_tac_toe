#include "init.h"

#define MAX_INIT_FUNCTIONS 100

struct init_subsystem_t {
  init_function functions[MAX_INIT_FUNCTIONS];
  size_t count;
};

struct init_subsystem_t init_subsystem = {.count = 0};

void register_init_function(init_function func) {
  if (init_subsystem.count < MAX_INIT_FUNCTIONS) {
    init_subsystem.functions[init_subsystem.count++] = func;
  } else {
    // Handle error: too many initialization functions
  }
}

void initialize_system() {
  for (size_t i = 0; i < init_subsystem.count; ++i) {
    init_subsystem.functions[i]();
  }
}
