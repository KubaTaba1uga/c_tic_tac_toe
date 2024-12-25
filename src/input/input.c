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

#include "static_array_lib.h"

// App's internal libs
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define INPUT_DEVICES_MAX 100

typedef struct InputSubsystem {
  SARRS_FIELD(devices, struct InputDevice, INPUT_DEVICES_MAX);
} InputSubsystem;

SARRS_DECL(InputSubsystem, devices, struct InputDevice, INPUT_DEVICES_MAX);

static struct LoggingUtilsOps *log_ops;
static struct InputSubsystem input_subsystem;

struct InputPrivateOps {
  int (*init)(struct InputSubsystem *);
  void (*destroy_registrar)(struct InputSubsystem *);
  int (*register_module)(struct InputAddDeviceInput *,
                         struct InputAddDeviceOutput *);
  int (*set_registration_callback)(struct InputSetCallbackInput *,
                                   struct InputSetCallbackOutput *);
  int (*start)(struct InputSubsystem *);
  int (*stop)(struct InputSubsystem *);
  int (*wait)(struct InputSubsystem *);
};

static struct InputPrivateOps *input_private_ops;
struct InputPrivateOps *get_input_private_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int input_init_system(void) {
  int err;

  log_ops = get_logging_utils_ops();
  input_private_ops = get_input_private_ops();

  log_ops->log_info(__FILE_NAME__, "Initializing input subsystem.");

  err = input_private_ops->init(&input_subsystem);
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
  input_private_ops->destroy_registrar(&input_subsystem);
  log_ops->log_info(__FILE_NAME__, "Input subsystem destroyed successfully.");
}

static int input_register_module_system(struct InputAddDeviceInput input,
                                        struct InputAddDeviceOutput *output) {
  int err;

  if (!output || !input.device)
    return EINVAL;

  input.private = &input_subsystem;

  err = input_private_ops->register_module(&input, output);
  if (err) {
    log_ops->log_info(__FILE_NAME__, "Module registration failed for: %s.",
                      input.device->display_name);
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Module registered successfully.");

  return 0;
}

static int
input_set_registration_callback_system(struct InputSetCallbackInput input,
                                       struct InputSetCallbackOutput *output) {
  int err;

  if (!output || !input.callback)
    return EINVAL;

  log_ops->log_info(__FILE_NAME__, "Setting a registration callback.");

  input.private = &input_subsystem;

  err = input_private_ops->set_registration_callback(&input, output);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Callback set successfully.");

  return 0;
}

static int input_start_system(void) {
  int err;

  log_ops->log_info(__FILE_NAME__, "Starting input subsystem.");

  err = input_private_ops->start(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem started successfully.");
  return 0;
}

static int input_stop_system(void) {
  int err;

  log_ops->log_info(__FILE_NAME__, "Stopping input subsystem.");

  err = input_private_ops->stop(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem stopped successfully.");
  return 0;
}

static int input_wait_system(void) {
  int err;

  log_ops->log_info(__FILE_NAME__, "Waiting for input subsystem.");

  err = input_private_ops->wait(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(__FILE_NAME__, "Input subsystem started successfully.");
  return 0;
}

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
static int input_init_input(struct InputSubsystem *subsystem) {
  InputSubsystem_devices_init(subsystem);

  return 0;
}

static void input_destroy_registrar(struct InputSubsystem *subsystem) {}

static int input_register_module(struct InputAddDeviceInput *input,
                                 struct InputAddDeviceOutput *output) {
  struct InputSubsystem *input_sys;
  int err;

  if (!output || !input->device) {
    return EINVAL;
  }

  output->device_id = -1;
  input_sys = input->private;

  err = InputSubsystem_devices_append(input_sys, *input->device);
  if (err) {
    return err;
  }

  output->device_id = InputSubsystem_devices_length(input_sys) - 1;

  log_ops->log_info(__FILE_NAME__, "Module registered successfully with ID: %d",
                    output->device_id);

  return 0;
}

static int
input_set_registration_callback(struct InputSetCallbackInput *input,
                                struct InputSetCallbackOutput *output) {
  struct InputSubsystem *input_sys;
  struct InputDevice *device;
  int err;

  if (!input || !output || !input->callback) {
    return EINVAL;
  }

  input_sys = input->private;

  err = InputSubsystem_devices_get(input_sys, input->device_id, &device);
  if (err) {
    log_ops->log_err(__FILE_NAME__, "Failed to retrieve device for ID %d: %s",
                     input->device_id, strerror(err));
    return err;
  }

  device->callback = input->callback;

  log_ops->log_info(__FILE_NAME__, "Callback set successfully for ID %d",
                    input->device_id);
  return 0;
}

static int input_start(struct InputSubsystem *subsystem) {
  struct InputDevice *device;
  size_t i;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (i = 0; i < InputSubsystem_devices_length(subsystem); i++) {
    err = InputSubsystem_devices_get(subsystem, i, &device);
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to get device for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    // If there is no callback set for device there is no point in starting it.
    if (!device->callback)
      continue;

    err = device->start();
    if (err) {
      log_ops->log_err(__FILE_NAME__, "Failed to start device '%d:%s': %s", i,
                       device->display_name, strerror(err));
      return err;
    }

    log_ops->log_err(__FILE_NAME__, "Started device '%d:%s'", i,
                     device->display_name);
  }

  return 0;
}

static int input_stop(struct InputSubsystem *subsystem) {
  struct InputDevice *device;
  size_t i;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (i = 0; i < InputSubsystem_devices_length(subsystem); i++) {
    err = InputSubsystem_devices_get(subsystem, i, &device);
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to get device for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    if (!device->callback)
      continue;

    err = device->stop();
    if (err) {
      log_ops->log_err(__FILE_NAME__, "Failed to stop device '%d:%s': %s", i,
                       device->display_name, strerror(err));
      return err;
    }

    // Once device is stopped we want to deregister callback.
    // This indicates that device stopped running.
    device->callback = NULL;

    log_ops->log_err(__FILE_NAME__, "Stoped device '%d:%s'", i,
                     device->display_name);
  }

  return 0;
}

static int input_wait(struct InputSubsystem *subsystem) {
  struct InputDevice *device;
  size_t i;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (i = 0; i < InputSubsystem_devices_length(subsystem); i++) {
    err = InputSubsystem_devices_get(subsystem, i, &device);
    if (err) {
      log_ops->log_err(__FILE_NAME__,
                       "Failed to get device for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    if (!device->callback)
      continue;

    err = device->wait();
    if (err) {
      log_ops->log_err(__FILE_NAME__, "Failed to wait for device '%d:%s': %s",
                       i, device->display_name, strerror(err));
      return err;
    }

    log_ops->log_err(__FILE_NAME__, "Waited for device '%d:%s'", i,
                     device->display_name);
  }

  return 0;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_input_reg = {
    .display_name = __FILE_NAME__,
    .init = input_init_system,
    .destroy = input_destroy_system,
};

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

static struct InputPrivateOps input_private_ops_ = {
    .start = input_start,
    .stop = input_stop,
    .wait = input_wait,
    .init = input_init_input,
    .destroy_registrar = input_destroy_registrar,
    .register_module = input_register_module,
    .set_registration_callback = input_set_registration_callback};

struct InputOps *get_input_ops(void) { return &input_ops; }

struct InputPrivateOps *get_input_private_ops(void) {
  return &input_private_ops_;
}
