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
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct InitSubsystem {
  array_t registrations;
};

struct InitPrivateOps {
  int (*init_registration)(struct InitRegistrationData *registration);
  void (*destroy_registration)(struct InitRegistrationData *registration);
};

static const size_t max_registrations = 100;
static const char module_id[] = "init_subsystem";

static struct InitPrivateOps *get_init_priv_ops(void);

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int init_init(init_t *init) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct InitSubsystem *tmp_init;
  int err;

  tmp_init = malloc(sizeof(struct InitSubsystem));
  if (!tmp_init) {
    return ENOMEM;
  }

  err = array_ops->init(tmp_init->registrations, max_registrations);
  if (err) {
    return err;
  }

  *init = tmp_init;

  return 0;
};

static void init_destroy(init_t *init) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct InitSubsystem *tmp_init = *init;

  array_ops->destroy(tmp_init->registrations);
  free(tmp_init);

  *init = NULL;
}

static int init_register(init_t init,
                         struct InitRegistrationData *registration_data) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct InitSubsystem *tmp_init = init;
  int err;

  if (array_ops->get_length(tmp_init->registrations) < max_registrations) {
    err = array_ops->append(tmp_init->registrations, registration_data);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to register %s in init, "
                           "unkown error.",
                           registration_data->id);
      return err;
    }
  } else {
    logging_ops->log_err(module_id,
                         "Unable to register %s in init, "
                         "no enough space in `registrations` array.",
                         registration_data->id);
    return ENOMEM;
  }

  return 0;
}

int init_initialize_registrations(init_t init) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct InitPrivateOps *init_ops = get_init_priv_ops();
  struct InitRegistrationData *registration_data;
  struct InitSubsystem *tmp_init = init;
  int err = 0;
  size_t i;

  for (i = 0; i < array_ops->get_length(tmp_init->registrations); ++i) {
    registration_data = array_ops->get_element(tmp_init->registrations, i);
    err = init_ops->init_registration(registration_data);
    if (err) {
      logging_ops->log_err(module_id, "Unable to initialize %s\n",
                           registration_data->id);
      return err;
    }
  }

  return 0;
}

void init_destroy_registrations(init_t init) {
  struct ArrayUtilsOps *array_ops = get_array_utils_ops();
  struct InitPrivateOps *init_ops = get_init_priv_ops();
  struct InitRegistrationData *registration_data;
  struct InitSubsystem *tmp_init = init;
  int i = array_ops->get_length(init);

  while (i--) {
    registration_data = array_ops->get_element(tmp_init->registrations, i);
    init_ops->destroy_registration(registration_data);
  }
}

int init_initialize_registration(struct InitRegistrationData *registration) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  int err;

  if (!registration->init)
    return 0;

  err = registration->init();
  if (err) {
    logging_ops->log_err(module_id, "Failed to initialize %s: %s",
                         registration->id, strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Initialized %s", registration->id);

  return 0;
}

void init_destroy_registration(struct InitRegistrationData *registration) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();

  if (!registration->destroy)
    return;

  logging_ops->log_info(module_id, "Destroying %s", registration->id);

  registration->destroy();
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InitPrivateOps init_priv_ops = {
    .init_registration = init_initialize_registration,
    .destroy_registration = init_destroy_registration};

static struct InitOps init_ops = {.init = init_init,
                                  .destroy = init_destroy,
                                  .register_module = init_register,
                                  .init_modules = init_initialize_registrations,
                                  .destroy_modules =
                                      init_destroy_registrations};

struct InitOps *get_init_ops(void) { return &init_ops; }

struct InitPrivateOps *get_init_priv_ops(void) { return &init_priv_ops; }
