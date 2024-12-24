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
/* #include "init/init.h" */
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define CONFIG_VARS_MAX 100

struct ConfigSubsystem {
  SAARS_FIELD(vars, struct ConfigVariable, CONFIG_VARS_MAX);
};

SAARS_DECL(struct ConfigSubsystem, vars, struct ConfigVariable,
           CONFIG_VARS_MAX);

struct ConfigPrivateOps {
  int (*init)(struct ConfigSubsystem *);
  int (*get_var)(struct ConfigGetVarInput *, struct ConfigGetVarOutput *);
  int (*add_var)(struct ConfigAddVarInput *, struct ConfigAddVarOutput *);
};

static struct ArrayUtilsOps *array_ops;
static struct LoggingUtilsOps *logging_ops;
static struct RegistrationUtilsOps *reg_ops;
static const size_t max_registrations = 100;
static struct ConfigSubsystem config_subsystem;

static struct ConfigPrivateOps *config_priv_ops;
struct ConfigPrivateOps *get_config_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int config_init_system(void) {
  int err;

  array_ops = get_array_utils_ops();
  logging_ops = get_logging_utils_ops();
  reg_ops = get_registration_utils_ops();
  config_priv_ops = get_config_priv_ops();

  err = config_priv_ops->init(&config_subsystem);
  if (err) {
    return err;
  }

  return 0;
}

static void config_destroy_system(void) {
  config_priv_ops->destroy(&config_subsystem);
}

static int config_add_variable_system(struct ConfigAddVarInput input,
                                      struct ConfigAddVarOutput *output) {
  struct ConfigAddVarOutput local_output;

  if (!output)
    output = &local_output;

  input.config = &config_subsystem;

  return config_priv_ops->add_variable(&input, output);
}

static int config_get_variable_system(struct ConfigGetVarInput input,
                                      struct ConfigGetVarOutput *output) {
  struct ConfigGetVarOutput local_output;

  if (!output)
    output = &local_output;

  input.config = &config_subsystem;

  return config_priv_ops->get_variable(&input, output);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int config_init(struct ConfigSubsystem *config) {
  if (!config)
    return EINVAL;

  SAARS_INIT(*config, vars);

  return 0;
}

static void config_destroy(struct ConfigSubsystem *config) {
  if (!config)
    return;
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
  /* struct ConfigSubsystem *config; */
  /* int err; */

  if (!input || !input->var || !output) {
    return EINVAL;
  }

  output->var_id = -1;
  /* config = input->config; */

  return 0;
}

static int config_get_variable(struct ConfigGetVarInput *input,
                               struct ConfigGetVarOutput *output) {
  /* struct ConfigSubsystem *config; */
  /* int err; */

  if (!input || !output) {
    return EINVAL;
  }

  output->var_name = NULL;
  output->value = NULL;

  /* config = input->config; */

  /* output->var_name = data->var_name; */
  /* output->value = getenv(data->var_name); */

  /* if (!output->value) */
  /*   output->value = data->default_value; */

  return 0;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_config_reg = {.data = {
                                               .display_name = __FILE_NAME__,
                                               .init = config_init_system,
                                               .destroy = config_destroy_system,
                                           }};

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
