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

#include <stddef.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
typedef int (*init_function_t)(void);
typedef void (*destroy_function_t)(void);

struct init_registration_data {
  init_function_t init_func;
  destroy_function_t destroy_func;
  const char *id;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct init_ops {
  void (*register_module)(struct init_registration_data init_registration_data);
  int (*initialize_system)(void);
  void (*destroy_system)(void);
};

extern struct init_ops init_ops;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define INIT_REGISTER_SUBSYSTEM(_init_func, _destroy_func, _id)                \
  static void _init_register() __attribute__((constructor));                   \
  static void _init_register() {                                               \
    struct init_registration_data init_registration_data = {                   \
        .init_func = _init_func, .destroy_func = _destroy_func, .id = _id};    \
    init_ops.register_module(init_registration_data);                          \
  }

#define INIT_REGISTER_SUBSYSTEM_PRIORITY(_init_func, _destroy_func, _id,       \
                                         _priority)                            \
  static void _init_register() __attribute__((constructor(_priority + 101)));  \
  static void _init_register() {                                               \
    struct init_registration_data init_registration_data = {                   \
        .init_func = _init_func, .destroy_func = _destroy_func, .id = _id};    \
    init_ops.register_module(init_registration_data);                          \
  }

#endif // INIT_SUBSYSTEM_H
