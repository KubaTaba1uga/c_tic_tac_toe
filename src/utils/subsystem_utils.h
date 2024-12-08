#ifndef SUBSYSTEM_UTILS_H
#define SUBSYSTEM_UTILS_H
/*******************************************************************************
 * @file subsystem_utils.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>

#include "utils/array_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
typedef struct Module *module_t;
typedef struct Subsystem *subsystem_t;

enum SubsystemStateEnum {
  SUBSYSTEM_STATE_ITER_INPROGRESS,
  SUBSYSTEM_STATE_ITER_DONE,
};

struct SubsystemState {
  enum SubsystemStateEnum state;
  void *private;
};

struct SubsystemGetModuleWrapper {
  void *filter_data;
  bool (*filter_func)(const char *, void *, void *);
  void **result_placeholder;
  struct SubsystemState *state;
};

struct SubsystemUtilsOps {
  int (*init)(subsystem_t *, const char *, const size_t);
  void (*destroy)(subsystem_t *);
  int (*register_module)(subsystem_t, const char *, void *);
  int (*get_module)(subsystem_t, struct SubsystemGetModuleWrapper *);
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData init_subsystem_utils_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct SubsystemUtilsOps *get_subsystem_utils_ops(void);

#endif // SUBSYSTEM_UTILS_H
