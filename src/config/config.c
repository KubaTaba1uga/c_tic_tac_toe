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
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "config.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_CONFIG_REGISTRATIONS 100

struct ConfigSubsystem {
  struct ConfigRegistrationData registrations[MAX_CONFIG_REGISTRATIONS];
  size_t count;
};

static const char module_id[] = "config_subsystem";

static struct ConfigSubsystem config_subsystem = {.count = 0};

static int config_register_variable(
    struct ConfigRegistrationData config_registration_data);
static char *config_get_variable(char *var_name);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static struct ConfigOps config_ops = {.register_var = config_register_variable,
                                      .get_var = config_get_variable};
struct ConfigOps *get_config_ops(void) { return &config_ops; }

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int config_register_variable(
    struct ConfigRegistrationData config_registration_data) {
  if (config_subsystem.count < MAX_CONFIG_REGISTRATIONS) {
    config_subsystem.registrations[config_subsystem.count++] =
        config_registration_data;
    return 0;
  }

  logging_utils_ops.log_err(module_id,
                            "Unable to register %s in config, "
                            "no enough space in `registrations` array.",
                            config_registration_data.var_name);
  return EINVAL;
}

char *config_get_variable(char *var_name) {
  char *value;
  size_t i;

  for (i = 0; i < config_subsystem.count; i++) {
    if (std_lib_utils_ops.are_str_eq(
            var_name, (char *)config_subsystem.registrations[i].var_name)) {
      value = getenv(var_name);

      if (value == NULL)
        value = (char *)config_subsystem.registrations[i].default_value;

      return value;
    }
  }

  logging_utils_ops.log_err(module_id,
                            "Unable to retreive var from config."
                            "%s not registered.",
                            var_name);

  return NULL;
}
