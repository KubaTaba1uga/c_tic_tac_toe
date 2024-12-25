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

// Third party libs
#include "static_array_lib.h"

// App's internal libs
#include "config/config.h"
#include "init/init.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define CONFIG_VARS_MAX 100

typedef struct ConfigSubsystem {
  SARRS_FIELD(vars, struct ConfigVariable, CONFIG_VARS_MAX);
} ConfigSubsystem;

SARRS_DECL(ConfigSubsystem, vars, struct ConfigVariable, CONFIG_VARS_MAX);

struct ConfigPrivateOps {
  int (*init)(struct ConfigSubsystem *);
  int (*get_var)(struct ConfigGetVarInput *, struct ConfigGetVarOutput *);
  int (*add_var)(struct ConfigAddVarInput *, struct ConfigAddVarOutput *);
};

static struct LoggingUtilsOps *logging_ops;
static struct ConfigSubsystem config_subsystem;

static struct ConfigPrivateOps *config_priv_ops;
struct ConfigPrivateOps *get_config_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int config_init_system(void) {
  int err;

  logging_ops = get_logging_utils_ops();
  config_priv_ops = get_config_priv_ops();

  err = config_priv_ops->init(&config_subsystem);
  if (err) {
    return err;
  }

  return 0;
}

static int config_add_variable_system(struct ConfigAddVarInput input,
                                      struct ConfigAddVarOutput *output) {
  struct ConfigAddVarOutput local_output;

  if (!output)
    output = &local_output;

  input.config = &config_subsystem;

  return config_priv_ops->add_var(&input, output);
}

static int config_get_variable_system(struct ConfigGetVarInput input,
                                      struct ConfigGetVarOutput *output) {
  struct ConfigGetVarOutput local_output;

  if (!output)
    output = &local_output;

  input.config = &config_subsystem;

  return config_priv_ops->get_var(&input, output);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int config_init(struct ConfigSubsystem *config) {
  if (!config)
    return EINVAL;

  ConfigSubsystem_vars_init(config);

  return 0;
}

static int config_var_init(struct ConfigVariable *var, char *var_name,
                           char *default_value) {
  if (!var || !var_name) {
    return EINVAL;
  }

  strncpy(var->var_name, var_name, CONFIG_VARIABLE_MAX - 1);
  var->var_name[CONFIG_VARIABLE_MAX - 1] = 0;

  if (default_value) {
    strncpy(var->default_value, default_value, CONFIG_VARIABLE_MAX - 1);
    var->var_name[CONFIG_VARIABLE_MAX - 1] = 0;
  } else {
    memset(var->default_value, 0, sizeof(char) * (CONFIG_VARIABLE_MAX - 1));
  }

  return 0;
};

static int config_add_variable(struct ConfigAddVarInput *input,
                               struct ConfigAddVarOutput *output) {
  struct ConfigSubsystem *config;
  int err;

  if (!input || !input->var || !output) {
    return EINVAL;
  }

  output->var_id = -1;
  config = input->config;

  err = ConfigSubsystem_vars_append(config, *input->var);
  if (err) {
    return err;
  }

  output->var_id = ConfigSubsystem_vars_length(config) - 1;

  return 0;
}

static int get_var_by_id(struct ConfigGetVarInput *input,
                         struct ConfigGetVarOutput *output) {
  struct ConfigVariable *var;
  int err;

  err = ConfigSubsystem_vars_get(&config_subsystem, input->var_id, &var);
  if (err) {
    return err;
  }

  output->var_id = input->var_id;
  output->var_name = var->var_name;
  output->value = getenv(var->var_name);

  if (!output->value) {
    output->value = var->default_value;
  }

  return 0;
}

static int get_var_by_name(struct ConfigGetVarInput *input,
                           struct ConfigGetVarOutput *output) {
  struct ConfigVariable *var;
  size_t i;
  int err;

  for (i = 0; i < ConfigSubsystem_vars_length(&config_subsystem); i++) {
    err = ConfigSubsystem_vars_get(&config_subsystem, i, &var);
    if (err) {
      return err;
    }

    if (strcmp(var->var_name, input->var_name) == 0) {
      output->var_id = i;
      output->var_name = var->var_name;
      output->value = getenv(var->var_name);

      if (!output->value) {
        output->value = var->default_value;
      }

      return 0;
    }
  }

  return ENOENT;
}

static int config_get_variable(struct ConfigGetVarInput *input,
                               struct ConfigGetVarOutput *output) {
  int (*get_var_funcs[])(struct ConfigGetVarInput *,
                         struct ConfigGetVarOutput *) = {
      [CONFIG_GET_VAR_BY_ID] = get_var_by_id,
      [CONFIG_GET_VAR_BY_NAME] = get_var_by_name,
  };

  if (!input || !output || input->mode <= CONFIG_GET_VAR_NONE ||
      input->mode >= CONFIG_GET_VAR_INVALID) {
    return EINVAL;
  }

  return get_var_funcs[input->mode](input, output);
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_config_reg = {
    .display_name = __FILE_NAME__,
    .init = config_init_system,
    .destroy = NULL,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct ConfigPrivateOps config_priv_ops_ = {
    .init = config_init,
    .get_var = config_get_variable,
    .add_var = config_add_variable,
};

static struct ConfigOps config_pub_ops = {
    .init_var = config_var_init,
    .get_var = config_get_variable_system,
    .add_var = config_add_variable_system,
};

struct ConfigOps *get_config_ops(void) { return &config_pub_ops; }

struct ConfigPrivateOps *get_config_priv_ops(void) { return &config_priv_ops_; }
