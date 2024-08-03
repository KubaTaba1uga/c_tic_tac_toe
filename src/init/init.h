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
  INIT_MODULE_ORDER_INPUT,
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
  void (*destroy_system)(void);
};

#define INIT_REGISTER_SUBSYSTEM(_init_registration_data, _priority)            \
  static void _init_register() __attribute__((constructor(101 + _priority)));  \
  static void _init_register() {                                               \
    init_ops.register_module(_init_registration_data);                         \
  }

#define INIT_REGISTER_SUBSYSTEM_CHILD(_child, _parent)                         \
  static void _init_register() __attribute__((constructor));                   \
  static void _init_register() {                                               \
    init_ops.register_child_module(_child, _parent);                           \
  }

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct InitOps init_ops;

#endif // INIT_SUBSYSTEM_H
