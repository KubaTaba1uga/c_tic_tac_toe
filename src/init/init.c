/*******************************************************************************
 * @file init.c
 * @brief This file contains the implementation of an initialization subsystem
 *        for managing the registration, initialization, and destruction of
 *        various subsystems within an application.
 *
 * The initialization subsystem allows for the registration of subsystems,
 * initializing them, and subsequently destroying them. It manages the order
 * and ensures proper error handling and logging.
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS
 ******************************************************************************/
#define MAX_INIT_REGISTRATIONS 100
static const char module_id[] = "init_subsystem";
struct init_subsystem_t {
  struct init_registration_data registrations[MAX_INIT_REGISTRATIONS];
  size_t count;
};
struct init_subsystem_t init_subsystem = {.count = 0};

static void
init_register_subsystem(struct init_registration_data init_registration_data);
static void init_register_child_subsystem(struct init_registration_data *parent,
                                          struct init_registration_data *child);
static int initialize_system(void);
static void destroy_system(void);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct init_ops init_ops = {.register_module = init_register_subsystem,
                            .register_child_module =
                                init_register_child_subsystem,
                            .initialize_system = initialize_system,
                            .destroy_system = destroy_system};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
void init_register_subsystem(
    struct init_registration_data init_registration_data) {
  if (init_subsystem.count < MAX_INIT_REGISTRATIONS) {
    init_subsystem.registrations[init_subsystem.count++] =
        init_registration_data;
  } else {
    logging_utils_ops.log_err(module_id,
                              "Unable to register %s in init, "
                              "no enough space in `registrations` array.",
                              init_registration_data.id);
  }
}

void init_register_child_subsystem(struct init_registration_data *parent,
                                   struct init_registration_data *child) {
  if (parent->child_count < INIT_MAX_CHILDREN) {
    parent->children[parent->child_count++] = child;
  } else {
    logging_utils_ops.log_err(module_id, "Max children reached for %s\n",
                              parent->id);
  }
}

int initialize_system(void) {
  int err = 0;

  size_t i;

  for (i = 0; i < init_subsystem.count; ++i) {
    if (init_subsystem.registrations[i].init_func == NULL)
      continue;

    err = init_subsystem.registrations[i].init_func();
    if (err) {
      logging_utils_ops.log_err(module_id, "Failed to initialize %s: %s",
                                init_subsystem.registrations[i].id,
                                strerror(err));
      return err;
    }

    logging_utils_ops.log_info(module_id, "Initialized %s",
                               init_subsystem.registrations[i].id);
  }

  return 0;
}

void destroy_system() {
  int i;

  for (i = init_subsystem.count; i >= 0; i--) {
    if (init_subsystem.registrations[i].destroy_func == NULL)
      continue;

    logging_utils_ops.log_info(module_id, "Destroying %s",
                               init_subsystem.registrations[i].id);

    init_subsystem.registrations[i].destroy_func();
  }
}
