// init_subsystem.h
#ifndef INIT_SUBSYSTEM_H
#define INIT_SUBSYSTEM_H

#include <stddef.h>

typedef int (*init_function)(void);

void register_init_function(init_function func);
void initialize_system();

// Macro to register an initialization function so it gets called during the
// program's initialization phase, before main() starts executing
#define REGISTER_INIT_FUNCTION(func)                                           \
  /* Declare a static function with a unique name by appending '_register' to  \
   * the given function name */                                                \
  /* The 'constructor' attribute ensures this function is called before main() \
   */                                                                          \
  static void func##_register() __attribute__((constructor));                  \
  /* Define the body of the unique function */                                 \
  static void func##_register() {                                              \
    /* Call the register_init_function to add the given function to the list   \
     * of initialization functions */                                          \
    register_init_function(func);                                              \
  }

#endif // INIT_SUBSYSTEM_H
