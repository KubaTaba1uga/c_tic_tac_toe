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
#include "config/config.h"
#include "init/init.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_CONFIG_REGISTRATIONS 100

struct ConfigSubsystem {
  size_t count;
  struct ConfigRegistrationData registrations[MAX_CONFIG_REGISTRATIONS];
};

static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;
static const char module_id[] = "config_subsystem";
static struct ConfigSubsystem config_subsystem = {.count = 0};

static int config_init(void);
static char *config_get_variable(char *var_name);
struct ConfigSubsystem *config_get_subsystem(void);
static int config_register_variable(
    struct ConfigRegistrationData config_registration_data);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct ConfigPrivateOps {
  struct ConfigSubsystem *(*get_subsystem)(void);
};

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
static struct InitRegistrationData init_config_reg = {
    .id = module_id,
    .init_func = config_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    API
 ******************************************************************************/
int config_init(void) {
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();

  return 0;
}

int config_register_variable(
    struct ConfigRegistrationData config_registration_data) {

  struct ConfigSubsystem *subsystem = config_priv_ops.get_subsystem();

  logging_ops->log_err(module_id, "Space in `registrations` array: %i",
                       subsystem->count);

  if (subsystem->count < MAX_CONFIG_REGISTRATIONS) {
    subsystem->registrations[subsystem->count++] = config_registration_data;
    return 0;
  }

  logging_ops->log_err(module_id,
                       "Unable to register %s in config, "
                       "no enough space in `registrations` array.",
                       config_registration_data.var_name);
  return EINVAL;
}

char *config_get_variable(char *var_name) {
  char *value;
  size_t i;

  struct ConfigSubsystem *subsystem = config_priv_ops.get_subsystem();

  for (i = 0; i < subsystem->count; i++) {
    if (std_lib_ops->are_str_eq(var_name,
                                (char *)subsystem->registrations[i].var_name)) {
      value = getenv(var_name);

      if (value == NULL)
        value = (char *)subsystem->registrations[i].default_value;

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

INIT_REGISTER_SUBSYSTEM(&init_config_reg, INIT_MODULE_ORDER_CONFIG);
