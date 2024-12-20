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
#include "utils/subsystem_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct ConfigSubsystem {
  subsystem_t subsystem;
};

typedef struct ConfigSubsystem *config_sys_t;

struct ConfigPrivateOps {
  int (*init)(config_sys_t *);
  void (*destroy)(config_sys_t *);
  char *(*get_variable)(config_sys_t, char *);
  bool (*search_filter)(const char *, void *, void *);
  int (*register_variable)(config_sys_t, struct ConfigRegistrationData *);
};

static config_sys_t config_subsystem;
static struct ArrayUtilsOps *array_ops;
static struct StdLibUtilsOps *std_lib_ops;
static struct LoggingUtilsOps *logging_ops;
static const size_t max_registrations = 100;
static struct SubsystemUtilsOps *subsystem_ops;
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
  subsystem_ops = get_subsystem_utils_ops();

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

static int config_init_obj(struct ConfigRegistrationData *reg_data,
                           char *var_name, char *default_value) {
  if (!reg_data || !var_name) {
    return EINVAL;
  }

  memset(reg_data, 0, sizeof(struct ConfigRegistrationData));

  strncpy(reg_data->var_name, var_name, CONFIG_VAR_NAME_MAX - 1);

  if (default_value) {
    strncpy(reg_data->default_value, default_value,
            CONFIG_DEFAULT_VALUE_MAX - 1);
  }

  return 0;
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

  if (!logging_ops || !subsystem_ops)
    return ENODATA;

  if (!config)
    return EINVAL;

  tmp_config = malloc(sizeof(struct ConfigSubsystem));
  if (!tmp_config) {
    logging_ops->log_err(
        module_id, "Unable to allocate memory for subsystem in config: %s",
        strerror(ENOMEM));
    return ENOMEM;
  }

  err =
      subsystem_ops->init(&tmp_config->subsystem, module_id, max_registrations);
  if (err) {
    logging_ops->log_err(module_id, "Unable to create subsystem in config: %s",
                         strerror(err));
    return err;
  }

  *config = tmp_config;

  return 0;
}

static void config_destroy(config_sys_t *config) {
  config_sys_t tmp_config;

  if (!subsystem_ops)
    return;

  if (!config || !*config)
    return;

  tmp_config = *config;

  subsystem_ops->destroy(&tmp_config->subsystem);

  *config = NULL;
}

static int config_register_variable(
    config_sys_t config,
    struct ConfigRegistrationData *config_registration_data) {
  int err;

  if (!subsystem_ops || !logging_ops)
    return ENODATA;

  if (!config || !config_registration_data)
    return EINVAL;

  err = subsystem_ops->register_module(config->subsystem,
                                       config_registration_data->var_name,
                                       config_registration_data);
  if (err) {
    return err;
  }

  return 0;
}

static bool config_array_search_filter(const char *tmp_var_name, void *_,
                                       void *target_var_name) {
  if (!std_lib_ops || !tmp_var_name || !target_var_name)
    return false;

  return std_lib_ops->are_str_eq((char *)tmp_var_name, (char *)target_var_name);
}

static char *config_get_variable(config_sys_t config, char *var_name) {
  struct ConfigRegistrationData *variable = NULL;
  module_search_t search_wrap;
  char *value;
  int err;

  if (!subsystem_ops || !logging_ops || !std_lib_ops || !config_priv_ops) {
    return NULL;
  }

  if (!config || !var_name) {
    return NULL;
  }

  err = subsystem_ops->init_search_module_wrapper(
      &search_wrap, var_name, config_array_search_filter, (void **)&variable);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to create search wrapper for"
                         "%s: %s",
                         var_name, strerror(err));
    return NULL;
  }

  err = subsystem_ops->search_modules(config->subsystem, search_wrap);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to search for"
                         "%s: %s",
                         var_name, strerror(err));
    subsystem_ops->destroy_search_module_wrapper(&search_wrap);
    return NULL;
  }

  if (subsystem_ops->get_state_search_module_wrapper(search_wrap) >=
      ARRAY_SEARCH_STATE_INVALID) {
    logging_ops->log_err(module_id,
                         "Invalid search result for"
                         "%s",
                         var_name, strerror(err));
    subsystem_ops->destroy_search_module_wrapper(&search_wrap);
    return NULL;
  };

  subsystem_ops->destroy_search_module_wrapper(&search_wrap);

  if (!variable) {
    logging_ops->log_err(module_id,
                         "Unable to retreive var from config:"
                         "%s not registered.",
                         var_name);
    return NULL;
  }

  value = getenv(variable->var_name);

  if (value == NULL)
    value = (char *)variable->default_value;

  if (strlen(value) == 0) {
    value = NULL;
  }

  return value;
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
    .search_filter = config_array_search_filter,
    .register_variable = config_register_variable,
};

static struct ConfigOps config_pub_ops = {
    .init = config_init_obj,
    .get_system_var = config_get_variable_system,
    .register_system_var = config_register_variable_system,
};

struct ConfigOps *get_config_ops(void) { return &config_pub_ops; }

struct ConfigPrivateOps *get_config_priv_ops(void) { return &config_priv_ops_; }
