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
#include "utils/registration_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct ConfigSubsystem {
  struct Registrar registrar;
};

struct ConfigPrivateOps {
  int (*init)(struct ConfigSubsystem *);
  void (*destroy)(struct ConfigSubsystem *);
  int (*get_variable)(struct ConfigGetVarInput *, struct ConfigGetVarOutput *);
  int (*register_variable)(struct ConfigRegisterVarInput *,
                           struct ConfigRegisterVarOutput *);
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

static int
config_register_variable_system(struct ConfigRegisterVarInput input,
                                struct ConfigRegisterVarOutput *output) {
  if (!input.registration || !output)
    return EINVAL;

  input.config = &config_subsystem;

  return config_priv_ops->register_variable(&input, output);
}

static int config_get_variable_system(struct ConfigGetVarInput input,
                                      struct ConfigGetVarOutput *output) {
  if (!output)
    return EINVAL;

  input.config = &config_subsystem;

  return config_priv_ops->get_variable(&input, output);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int config_init(struct ConfigSubsystem *config) {
  int err;
  if (!config)
    return EINVAL;

  err = reg_ops->init(&config->registrar, __FILE__, max_registrations);
  if (err) {
    logging_ops->log_err(__FILE__, "Unable to create registrar: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

static void config_destroy(struct ConfigSubsystem *config) {
  if (!config)
    return;

  reg_ops->destroy(&config->registrar);
}

static int config_registration_init(struct ConfigRegistration *registration,
                                    char *var_name, char *default_value) {
  int err;
  if (!registration || !var_name) {
    return EINVAL;
  }

  err = reg_ops->registration_init(&registration->registration, var_name,
                                   &registration->data);
  if (err) {
    logging_ops->log_err(__FILE__, "Unable to init '%s' registration",
                         registration->data.var_name);
    return ENOMEM;
  }

  registration->data.default_value = default_value;

  // registration_init copy the var_name into display_name
  //  so we need to sync pointers to point to one str.
  registration->data.var_name = registration->registration.display_name;

  return 0;
};

static int config_register_variable(struct ConfigRegisterVarInput *input,
                                    struct ConfigRegisterVarOutput *output) {
  struct RegisterOutput reg_output;
  struct RegisterInput reg_input;
  struct ConfigSubsystem *config;
  int err;

  if (!input || !output) {
    return EINVAL;
  }

  output->registration_id = -1;

  config = input->config;

  reg_input.registration = &input->registration->registration;
  reg_input.registrar = &config->registrar;

  err = reg_ops->register_module(reg_input, &reg_output);
  if (err) {
    logging_ops->log_err(__FILE__, "Unable to register module: %s",
                         strerror(err));
    return err;
  }

  output->registration_id = reg_output.registration_id;

  return 0;
}

static int config_get_variable(struct ConfigGetVarInput *input,
                               struct ConfigGetVarOutput *output) {
  struct GetRegistrationOutput reg_output;
  struct GetRegistrationInput reg_input;
  struct ConfigRegistrationData *data;
  struct ConfigSubsystem *config;
  int err;

  if (!input || !output) {
    return EINVAL;
  }

  output->var_name = NULL;
  output->value = NULL;

  config = input->config;

  reg_input.registration_id = input->registration_id;
  reg_input.registrar = &config->registrar;

  err = reg_ops->get_registration(reg_input, &reg_output);
  if (err) {
    logging_ops->log_err(__FILE__,
                         "Unable to get"
                         "'%i' registration: %s",
                         input->registration_id, strerror(err));
    return err;
  }

  data = reg_output.registration->private;

  output->var_name = data->var_name;
  output->value = getenv(data->var_name);

  if (!output->value)
    output->value = data->default_value;

  return 0;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_config_reg = {.data = {
                                               .display_name = __FILE__,
                                               .init = config_init_system,
                                               .destroy = config_destroy_system,
                                           }};

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
    .registration_init = config_registration_init,
    .get_system_var = config_get_variable_system,
    .register_system_var = config_register_variable_system,
};

struct ConfigOps *get_config_ops(void) { return &config_pub_ops; }

struct ConfigPrivateOps *get_config_priv_ops(void) { return &config_priv_ops_; }
