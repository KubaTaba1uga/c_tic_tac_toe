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
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_INIT_REGISTRATIONS 100

struct InitSubsystem {
  struct InitRegistrationData *registrations[MAX_INIT_REGISTRATIONS];
  size_t count;
};

static const char module_id[] = "init_subsystem";
struct InitSubsystem init_subsystem = {.count = 0};

static void
init_register_subsystem(struct InitRegistrationData *init_registration_data);
static void init_register_child_subsystem(struct InitRegistrationData *child,
                                          struct InitRegistrationData *parent);
static int init_initialize_system(void);
static void init_destroy_system(void);
static int init_initialize_subsystem(struct InitRegistrationData *subsystem);
static void init_destroy_subsystem(struct InitRegistrationData *subsystem);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct InitOps init_ops = {.register_module = init_register_subsystem,
                           .register_child_module =
                               init_register_child_subsystem,
                           .initialize_system = init_initialize_system,
                           .destroy_system = init_destroy_system};
struct InitOps *get_init_ops(void) { return &init_ops; }

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void init_register_subsystem(
    struct InitRegistrationData *init_registration_data) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();

  if (init_subsystem.count < MAX_INIT_REGISTRATIONS) {
    init_subsystem.registrations[init_subsystem.count++] =
        init_registration_data;
  } else {
    logging_ops->log_err(module_id,
                         "Unable to register %s in init, "
                         "no enough space in `registrations` array.",
                         init_registration_data->id);
  }
}

void init_register_child_subsystem(struct InitRegistrationData *child,
                                   struct InitRegistrationData *parent) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();

  if (parent->child_count < INIT_MAX_CHILDREN) {
    parent->children[parent->child_count++] = child;
  } else {
    logging_ops->log_err(module_id, "Max children reached for %s\n",
                         parent->id);
  }
}

int init_initialize_system(void) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  int err = 0;
  size_t i;

  for (i = 0; i < init_subsystem.count; ++i) {
    if (init_subsystem.registrations[i]->init_func == NULL)
      continue;

    err = init_initialize_subsystem(init_subsystem.registrations[i]);
    if (err) {
      logging_ops->log_err(module_id, "Unable to initialize %s\n",
                           init_subsystem.registrations[i]->id);

      return err;
    }
  }

  return 0;
}

void init_destroy_system() {
  int i;

  for (i = init_subsystem.count - 1; i >= 0; --i) {
    if (init_subsystem.registrations[i]->destroy_func == NULL)
      continue;

    init_destroy_subsystem(init_subsystem.registrations[i]);
  }
}

int init_initialize_subsystem(struct InitRegistrationData *subsystem) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  size_t i;
  int err;

  if (subsystem->child_count != 0) {
    for (i = 0; i < subsystem->child_count; ++i) {
      err = init_initialize_subsystem(subsystem->children[i]);
      if (err != 0)
        return err;
    }
  }

  err = subsystem->init_func();
  if (err) {
    logging_ops->log_err(module_id, "Failed to initialize %s: %s",
                         subsystem->id, strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Initialized %s", subsystem->id);

  return 0;
}

void init_destroy_subsystem(struct InitRegistrationData *subsystem) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  size_t i;

  if (subsystem->child_count != 0) {
    for (i = 0; i < subsystem->child_count; ++i) {
      init_destroy_subsystem(subsystem->children[i]);
    }
  }

  logging_ops->log_info(module_id, "Destroying %s", subsystem->id);

  subsystem->destroy_func();
}
