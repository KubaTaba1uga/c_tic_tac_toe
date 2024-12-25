/*******************************************************************************
 * @file input.c
 * @brief Public API for input subsystem using private operations.
 *
 * This file contains the implementation of the input subsystem's public API,
 * including initialization, destruction, registration, and callback management.
 * The subsystem relies on logging and registration utilities to ensure robust
 * and traceable operations.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "input/input.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct InputSubsystem {
  struct Registrar registrar;
};

static struct LoggingUtilsOps *log_ops;
static struct RegistrationUtilsOps *reg_ops;
static struct InputSubsystem input_subsystem;

struct InputPrivateOps {
  int (*init_registrar)(struct InputSubsystem *);
  void (*destroy_registrar)(struct InputSubsystem *);
  int (*register_module)(struct InputRegisterInput,
                         struct InputRegisterOutput *);
  int (*set_registration_callback)(struct InputSetRegistrationCallbackInput,
                                   struct InputSetRegistrationCallbackOutput *);
  int (*start)(struct InputSubsystem *);
  int (*stop)(struct InputSubsystem *);
  int (*wait)(struct InputSubsystem *);
};

static struct InputPrivateOps *private_ops;
struct InputPrivateOps *get_private_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int input_init_system(void) {
  int err;

  log_ops = get_logging_utils_ops();
  reg_ops = get_registration_utils_ops();
  private_ops = get_private_ops();

  log_ops->log_info(__FILE_NAME__, "Initializing input subsystem.");

  err = private_ops->init_registrar(&input_subsystem);
  if (err) {
    log_ops->log_err(__FILE_NAME__, "Failed to initialize registrar: %s",
                     strerror(err));
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem initialized successfully.");

  return 0;
}

static void input_destroy_system(void) {
  log_ops->log_info(__FILE_NAME__, "Destroying input subsystem.");
  private_ops->destroy_registrar(&input_subsystem);
  log_ops->log_info(__FILE_NAME__, "Input subsystem destroyed successfully.");
}

static int input_register_module_system(struct InputRegisterInput input,
                                        struct InputRegisterOutput *output) {
  int err;

  if (!output || !input.registration)
    return EINVAL;

  log_ops->log_info(__FILE_NAME__, "Registering a module.");

  err = private_ops->register_module(input, output);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Module registered successfully.");

  return 0;
}

static int input_set_registration_callback_system(
    struct InputSetRegistrationCallbackInput input,
    struct InputSetRegistrationCallbackOutput *output) {
  int err;

  if (!output || !input.callback)
    return EINVAL;

  log_ops->log_info(__FILE_NAME__, "Setting a registration callback.");

  err = private_ops->set_registration_callback(input, output);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Callback set successfully.");

  return 0;
}

static int input_start_system(void) {
  int err;

  log_ops->log_info(__FILE_NAME__, "Starting input subsystem.");

  err = private_ops->start(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem started successfully.");
  return 0;
}

static int input_stop_system(void) {
  int err;

  log_ops->log_info(__FILE_NAME__, "Stopping input subsystem.");

  err = private_ops->stop(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem stopped successfully.");
  return 0;
}

static int input_wait_system(void) {
  int err;

  log_ops->log_info(__FILE_NAME__, "Waiting for input subsystem.");

  err = private_ops->wait(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem started successfully.");
  return 0;
}

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
static int input_init_registrar(struct InputSubsystem *subsystem) {
  log_ops->log_info(__FILE_NAME__, "Initializing registrar.");
  return reg_ops->init(&subsystem->registrar, __FILE_NAME__, 10);
}

static void input_destroy_registrar(struct InputSubsystem *subsystem) {
  log_ops->log_info(__FILE_NAME__, "Destroying registrar.");
  if (subsystem->registrar.registrations.data) {
    reg_ops->destroy(&subsystem->registrar);
  }
}

static int input_register_module(struct InputRegisterInput input,
                                 struct InputRegisterOutput *output) {
  struct RegisterOutput reg_output;
  int err;

  log_ops->log_info(__FILE_NAME__, "Starting module registration.");

  if (!output) {
    log_ops->log_err(__FILE_NAME__, "Output structure is NULL.");
    return EINVAL;
  }

  err = reg_ops->register_module(
      (struct RegisterInput){.registration = &input.registration->registration,
                             .registrar = &input_subsystem.registrar},
      &reg_output);
  if (err) {
    log_ops->log_err(__FILE_NAME__, "Failed to register module: %s",
                     strerror(err));
    return err;
  }

  output->registration_id = reg_output.registration_id;

  log_ops->log_info(__FILE_NAME__, "Module registered successfully with ID: %d",
                    output->registration_id);

  return 0;
}

static int input_set_registration_callback(
    struct InputSetRegistrationCallbackInput input,
    struct InputSetRegistrationCallbackOutput *output) {
  struct GetRegistrationOutput get_output;
  int err;

  log_ops->log_info(__FILE_NAME__, "Setting registration callback.");

  if (!output) {
    log_ops->log_err(__FILE_NAME__, "Output structure is NULL.");
    return EINVAL;
  }

  err = reg_ops->get_registration(
      (struct GetRegistrationInput){
          .registrar = &input_subsystem.registrar,
          .registration_id = input.registration_id,
      },
      &get_output);
  if (err) {
    log_ops->log_err(__FILE_NAME__,
                     "Failed to retrieve registration for ID %d: %s",
                     input.registration_id, strerror(err));
    return err;
  }

  struct InputRegistrationData *reg_data = get_output.registration->private;
  reg_data->callback = input.callback;

  log_ops->log_info(__FILE_NAME__, "Callback set successfully for ID %d.",
                    input.registration_id);
  return 0;
}

static int input_start(struct InputSubsystem *subsystem) {
  struct GetRegistrationOutput get_output;
  struct InputRegistrationData *reg_data;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (int i = 0; i < subsystem->registrar.registrations.length; i++) {
    err = reg_ops->get_registration(
        (struct GetRegistrationInput){.registration_id = i,
                                      .registrar = &subsystem->registrar},
        &get_output);
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to get registration for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    reg_data = get_output.registration->private;
    if (!reg_data->callback)
      continue;

    err = reg_data->start();
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to start registration for module  '%d:%s': %s",
                       i, reg_data->display_name, strerror(err));
      return err;
    }
  }

  return 0;
}

static int input_stop(struct InputSubsystem *subsystem) {
  struct GetRegistrationOutput get_output;
  struct InputRegistrationData *reg_data;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (int i = 0; i < subsystem->registrar.registrations.length; i++) {
    err = reg_ops->get_registration(
        (struct GetRegistrationInput){.registration_id = i,
                                      .registrar = &subsystem->registrar},
        &get_output);
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to get registration for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    reg_data = get_output.registration->private;
    if (!reg_data->callback)
      continue;

    err = reg_data->stop();
    if (err) {
      log_ops->log_err(__FILE_NAME__, "Failed to stop module '%d:%s': %s", i,
                       reg_data->display_name, strerror(err));
      return err;
    }

    reg_data->callback = NULL;
  }

  return 0;
}

static int input_wait(struct InputSubsystem *subsystem) {
  struct GetRegistrationOutput get_output;
  struct InputRegistrationData *reg_data;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (int i = 0; i < subsystem->registrar.registrations.length; i++) {
    err = reg_ops->get_registration(
        (struct GetRegistrationInput){.registration_id = i,
                                      .registrar = &subsystem->registrar},
        &get_output);
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to get registration for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    reg_data = get_output.registration->private;
    if (!reg_data->callback)
      continue;

    err = reg_data->wait();
    if (err) {
      log_ops->log_err(__FILE_NAME__, "Failed to wait for module '%d:%s': %s",
                       i, reg_data->display_name, strerror(err));
      return err;
    }
  }

  return 0;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_input_reg = {
    .data = {.display_name = __FILE_NAME__,
             .init = input_init_system,
             .destroy = input_destroy_system}};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputOps input_ops = {
    .start = input_start_system,
    .stop = input_stop_system,
    .wait = input_wait_system,
    .register_module = input_register_module_system,
    .set_callback = input_set_registration_callback_system,
};

static struct InputPrivateOps private_ops_ = {
    .start = input_start,
    .stop = input_stop,
    .wait = input_wait,
    .init_registrar = input_init_registrar,
    .destroy_registrar = input_destroy_registrar,
    .register_module = input_register_module,
    .set_registration_callback = input_set_registration_callback};

struct InputOps *get_input_ops(void) { return &input_ops; }

struct InputPrivateOps *get_private_ops(void) { return &private_ops_; }
