#ifndef INIT_SUBSYSTEM_H
#define INIT_SUBSYSTEM_H
/*******************************************************************************
 * @file init.h
 * @brief Initialization subsystem for setting up all necessary data.
 *
 * The init subsystem is responsible for initializing all data used
 *  for the entire lifetime of the program.
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>
#include <stdio.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define INIT_MAX_CHILDREN 20

// This enum manages initialization ordering
enum InitModuleOrderNumber {
  INIT_MODULE_ORDER_LOGGING,
  INIT_MODULE_ORDER_CONFIG,
  INIT_MODULE_ORDER_INPUT,
  INIT_MODULE_ORDER_GAME,
};

typedef int (*init_function_t)(void);
typedef void (*destroy_function_t)(void);

struct InitRegistrationData {
  init_function_t init_func;
  destroy_function_t destroy_func;
  const char *id;
  struct InitRegistrationData *children[INIT_MAX_CHILDREN];
  int child_count;
};

struct InitOps {
  void (*register_module)(struct InitRegistrationData *init_registration_data);
  void (*register_child_module)(struct InitRegistrationData *child,
                                struct InitRegistrationData *parent);
  int (*initialize_system)(void);
  int (*initialize_system_with_disabled_modules)(size_t n,
                                                 char *modules_ids[n]);
  void (*destroy_system)(void);
};

// We are using line of definition to generate random function names, it is
// especially usefull when importing multiple .c files into test.
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)
#define UNIQUE_NAME(base) CONCAT(base, __LINE__)

#define INIT_REGISTER_SUBSYSTEM(_init_registration_data, _priority)            \
  static void UNIQUE_NAME(_init_register)()                                    \
      __attribute__((constructor(101 + _priority)));                           \
  static void UNIQUE_NAME(_init_register)() {                                  \
    struct InitOps *init_ops = get_init_ops();                                 \
    if (!init_ops) {                                                           \
      fprintf(stderr, "Failed to get init_ops\n");                             \
    }                                                                          \
    if (!init_ops->register_module) {                                          \
      fprintf(stderr, "register_module function is not set\n");                \
    }                                                                          \
    init_ops->register_module(_init_registration_data);                        \
  }

#define INIT_REGISTER_SUBSYSTEM_CHILD(_child, _parent)                         \
  static void UNIQUE_NAME(_init_register)() __attribute__((constructor));      \
  static void UNIQUE_NAME(_init_register)() {                                  \
    struct InitOps *init_ops = get_init_ops();                                 \
    init_ops->register_child_module(_child, _parent);                          \
  }

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InitOps *get_init_ops(void);

#endif // INIT_SUBSYSTEM_H
