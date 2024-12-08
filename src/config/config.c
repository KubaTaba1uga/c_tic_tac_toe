/*******************************************************************************
 * @file config.c
 * @brief This file contains the implementation of a configuration subsystem
 *        that manages the registration and retrieval of configuration
variables.
 *
 * The configuration subsystem allows for the registration of configuration
 * variables, storing them internally, and retrieving their values either from
 * environment variables or default values.
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <asm-generic/errno.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "init/init.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct ConfigSubsystem {
  array_t registrations;
};

typedef struct ConfigSubsystem *config_t;

struct ConfigPrivateOps {
  struct ConfigSubsystem *(*get_subsystem)(void);
};

static struct ArrayUtilsOps *array_ops;
static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;
static const size_t max_registrations = 100;
static struct ConfigSubsystem config_subsystem;
static const char module_id[] = "config_subsystem";

static int config_init(void);
static void config_destroy(void);
static char *config_get_variable(char *var_name);
struct ConfigSubsystem *config_get_subsystem(void);
static int config_register_variable(
    struct ConfigRegistrationData *config_registration_data);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ConfigPrivateOps config_priv_ops = {
    .get_subsystem = config_get_subsystem,
};

static struct ConfigOps config_ops = {.register_var = config_register_variable,
                                      .get_var = config_get_variable,
                                      .private_ops = &config_priv_ops};

struct ConfigOps *get_config_ops(void) { return &config_ops; }

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_config_reg = {
    .id = module_id,
    .init = config_init,
    .destroy = config_destroy,
};

/*******************************************************************************
 *    API
 ******************************************************************************/
int config_init(void) {
  int err;
  array_ops = get_array_utils_ops();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();

  err = array_ops->init(&config_subsystem.registrations, max_registrations);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to create array in config module: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

void config_destroy(void) {
  if (!array_ops)
    return;

  array_ops->destroy(&config_subsystem.registrations);
}

int config_register_variable(
    struct ConfigRegistrationData *config_registration_data) {

  struct ConfigSubsystem *subsystem = config_priv_ops.get_subsystem();

  if (!array_ops || !logging_ops)
    return ENODATA;

  if (array_ops->get_length(subsystem->registrations) < max_registrations) {
    array_ops->append(subsystem->registrations, config_registration_data);
    return 0;
  }

  logging_ops->log_err(module_id,
                       "Unable to register %s in config, "
                       "no enough space in `registrations` array.",
                       config_registration_data->var_name);
  return EINVAL;
}

char *config_get_variable(char *var_name) {
  struct ConfigRegistrationData *variable;
  char *value;
  size_t i;

  struct ConfigSubsystem *subsystem = config_priv_ops.get_subsystem();

  for (i = 0; i < array_ops->get_length(subsystem->registrations); i++) {
    variable = array_ops->get_element(subsystem->registrations, i);
    if (std_lib_ops->are_str_eq(var_name, (char *)variable->var_name)) {
      value = getenv(var_name);

      if (value == NULL)
        value = (char *)variable->default_value;

      return value;
    }
  }

  logging_ops->log_err(module_id,
                       "Unable to retreive var from config."
                       "%s not registered.",
                       var_name);

  return NULL;
}

struct ConfigSubsystem *config_get_subsystem(void) { return &config_subsystem; }
