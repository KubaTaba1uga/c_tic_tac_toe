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
#include <asm-generic/errno-base.h>
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

typedef struct ConfigSubsystem *config_sys_t;

struct ConfigPrivateOps {
  int (*init)(config_sys_t *);
  void (*destroy)(config_sys_t *);
  char *(*get_variable)(config_sys_t, char *);
  int (*register_variable)(config_sys_t, struct ConfigRegistrationData *);
};

static config_sys_t config_subsystem;
static struct ArrayUtilsOps *array_ops;
static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;
static const size_t max_registrations = 100;
static const char module_id[] = "config_subsystem";

static struct ConfigPrivateOps *config_priv_ops;
struct ConfigPrivateOps *get_config_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int config_init_system(void) {
  int err;

  array_ops = get_array_utils_ops();
  logging_ops = get_logging_utils_ops();
  std_lib_ops = get_std_lib_utils_ops();
  config_priv_ops = get_config_priv_ops();

  err = config_priv_ops->init(&config_subsystem);
  if (err) {
    return err;
  }

  return 0;
}

static void config_destroy_system(void) {
  if (!config_priv_ops)
    return;

  config_priv_ops->destroy(&config_subsystem);
}

static int config_register_variable_system(
    struct ConfigRegistrationData *config_registration_data) {
  if (!config_priv_ops)
    return ENODATA;

  return config_priv_ops->register_variable(config_subsystem,
                                            config_registration_data);
}

static char *config_get_variable_system(char *var_name) {
  if (!config_priv_ops)
    return NULL;

  return config_priv_ops->get_variable(config_subsystem, var_name);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int config_init(config_sys_t *config) {
  config_sys_t tmp_config;
  int err;

  if (!logging_ops || !array_ops)
    return ENODATA;

  if (!config)
    return EINVAL;

  tmp_config = malloc(sizeof(struct ConfigSubsystem));
  if (!tmp_config) {
    logging_ops->log_err(module_id,
                         "Unable to create subsystem in config module: %s",
                         strerror(ENOMEM));
    return ENOMEM;
  }

  err = array_ops->init(&tmp_config->registrations, max_registrations);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to create array in config module: %s",
                         strerror(err));
    return err;
  }

  *config = tmp_config;

  return 0;
}

static void config_destroy(config_sys_t *config) {
  config_sys_t tmp_config;

  if (!array_ops)
    return;

  if (!config || !*config)
    return;

  tmp_config = *config;

  array_ops->destroy(&tmp_config->registrations);

  *config = NULL;
}

static int config_register_variable(
    config_sys_t config,
    struct ConfigRegistrationData *config_registration_data) {
  int err;

  if (!array_ops || !logging_ops)
    return ENODATA;

  if (!config || !config_registration_data)
    return EINVAL;

  err = array_ops->append(config->registrations, config_registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to register %s in config: %s",
                         config_registration_data->var_name, strerror(err));
    return err;
  }

  return 0;
}

static char *config_get_variable(config_sys_t config, char *var_name) {
  struct ConfigRegistrationData *variable;
  char *value;
  size_t i;

  if (!array_ops || !logging_ops || !std_lib_ops) {
    return NULL;
  }

  if (!config || !var_name) {
    return NULL;
  }

  for (i = 0; i < array_ops->get_length(config->registrations); i++) {
    variable = array_ops->get_element(config->registrations, i);
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

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_config_reg = {
    .id = module_id,
    .init = config_init_system,
    .destroy = config_destroy_system,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ConfigPrivateOps config_priv_ops_ = {
    .init = config_init,
    .destroy = config_destroy,
    .get_variable = config_get_variable,
    .register_variable = config_register_variable,
};

static struct ConfigOps config_pub_ops = {
    .get_system_var = config_get_variable_system,
    .register_system_var = config_register_variable_system,
};

struct ConfigOps *get_config_ops(void) { return &config_pub_ops; }

struct ConfigPrivateOps *get_config_priv_ops(void) { return &config_priv_ops_; }
