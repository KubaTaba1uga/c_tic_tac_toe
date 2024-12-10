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

#include "array_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
typedef struct Module *module_t;
typedef struct ModuleSearchWrapper *module_search_t;
typedef enum ArraySearchStateEnum module_search_enum_t;
typedef struct Subsystem *subsystem_t;

struct SubsystemUtilsOps {
  int (*init)(subsystem_t *, const char *, const size_t);
  void (*destroy)(subsystem_t *);
  int (*register_module)(subsystem_t, const char *, void *);
  int (*search_modules)(subsystem_t, module_search_t);
  int (*init_search_module_wrapper)(module_search_t *, void *,
                                    bool (*)(const char *, void *, void *),
                                    void **);
  void (*destroy_search_module_wrapper)(module_search_t *);
  module_search_enum_t (*get_state_search_module_wrapper)(module_search_t);
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
