/*******************************************************************************
 * @file input.c
 * @brief Input subsystem implementation.
 *
 * This subsystem manages input devices and their associated callbacks.
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
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct InputSubsystem {
  struct Registrar registrar;
};

static struct InputSubsystem input_subsystem;
static struct LoggingUtilsOps *log_ops;
static struct RegistrationUtilsOps *reg_ops;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int input_init_system(void) {
  int err;
  log_ops = get_logging_utils_ops();
  reg_ops = get_registration_utils_ops();

  err = reg_ops->init(&input_subsystem.registrar, __FILE_NAME__, 10);
  if (err) {
    log_ops->log_err("input", "Failed to initialize registrar: %s",
                     strerror(err));
    return err;
  }

  log_ops->log_info("input", "Input subsystem initialized successfully");
  return 0;
}

static void input_destroy_system(void) {
  if (input_subsystem.registrar.registrations.data) {
    reg_ops->destroy(&input_subsystem.registrar);
    log_ops->log_info("input", "Input subsystem destroyed successfully");
  }
}

static int input_register_callback(struct InputRegisterVarInput input,
                                   struct InputRegisterVarOutput *output) {
  struct InputSubsystem *tmp_input;
  struct RegisterOutput reg_output;
  struct RegisterInput reg_input;

  if (!output) {
    return EINVAL;
  }

  input.input = tmp_input = &input_subsystem;

  int err = reg_ops->registration_init(&input.registration->registration,
                                       input.registration->data.display_name,
                                       input.registration->data.callback);
  if (err) {
    log_ops->log_err("input", "Failed to initialize registration for '%s': %s",
                     input.registration->data.display_name, strerror(err));
    return err;
  }

  reg_input.registrar = &tmp_input->registrar;

  err = reg_ops->register_module(reg_input, &reg_output);
  if (err) {
    log_ops->log_err("input", "Failed to register callback for '%s': %s",
                     input.registration->data.display_name, strerror(err));
    return err;
  }

  log_ops->log_info("input", "Callback registered successfully for '%s'",
                    input.registration->data.display_name);
  return 0;
}

static int input_start(void) {
  log_ops->log_info("input", "Input subsystem started");
  return 0;
}

static int input_stop(void) {
  log_ops->log_info("input", "Input subsystem stopped");
  return 0;
}

static int input_wait(void) {
  log_ops->log_info("input", "Input subsystem waiting");
  return 0;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_input_reg = {
    .display_name = "Input Subsystem",
    .init = input_init_system,
    .destroy = input_destroy_system,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputOps input_ops = {
    .start = input_start,
    .stop = input_stop,
    .wait = input_wait,
    .register_callback = input_register_callback,
};

struct InputOps *get_input_ops(void) { return &input_ops; }
