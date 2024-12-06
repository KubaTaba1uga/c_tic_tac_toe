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
 *    API
 ******************************************************************************/
typedef int (*init_init_function_t)(void);
typedef void (*init_destroy_function_t)(void);
typedef void *init_t;

struct InitRegistrationData {
  const char *id;
  init_init_function_t init;
  init_destroy_function_t destroy;
};

struct InitOps {
  int (*init)(init_t *);
  void (*destroy)(init_t *);
  int (*register_module)(init_t init, struct InitRegistrationData *);
  int (*init_modules)(init_t init);
  void (*destroy_modules)(init_t init);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InitOps *get_init_ops(void);

#endif // INIT_SUBSYSTEM_H
