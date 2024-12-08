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
 *    API
 ******************************************************************************/
static int config_init_subsystem(void) {
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

static int config_init(config_sys_t *config) {
  config_sys_t tmp_config;
  int err;

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

static void config_destroy_subsystem(void) {
  if (!config_priv_ops)
    return;

  config_priv_ops->destroy(&config_subsystem);
}

static void config_destroy(config_sys_t *config) {
  config_sys_t tmp_config;

  if (!array_ops || !config || !*config)
    return;

  tmp_config = *config;

  array_ops->destroy(&tmp_config->registrations);

  *config = NULL;
}

static int config_register_variable(
    struct ConfigRegistrationData *config_registration_data) {
  if (!array_ops || !logging_ops || !config_subsystem)
    return ENODATA;

  if (array_ops->get_length(config_subsystem->registrations) <
      max_registrations) {
    array_ops->append(config_subsystem->registrations,
                      config_registration_data);
    return 0;
  }

  logging_ops->log_err(module_id,
                       "Unable to register %s in config, "
                       "no enough space in `registrations` array.",
                       config_registration_data->var_name);
  return EINVAL;
}

static char *config_get_variable(char *var_name) {
  struct ConfigRegistrationData *variable;
  char *value;
  size_t i;

  if (!array_ops || !logging_ops || !std_lib_ops || !config_subsystem) {
    return NULL;
  }

  for (i = 0; i < array_ops->get_length(config_subsystem->registrations); i++) {
    variable = array_ops->get_element(config_subsystem->registrations, i);
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
    .init = config_init_subsystem,
    .destroy = config_destroy_subsystem,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ConfigPrivateOps config_priv_ops_ = {
    .init = config_init,
    .destroy = config_destroy,
};

static struct ConfigOps config_pub_ops = {
    .register_var = config_register_variable,
    .get_var = config_get_variable,
};

struct ConfigOps *get_config_ops(void) { return &config_pub_ops; }

struct ConfigPrivateOps *get_config_priv_ops(void) { return &config_priv_ops_; }
