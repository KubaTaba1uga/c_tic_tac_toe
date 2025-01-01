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
  int (*add_device)(struct InputAddDeviceInput *,
                    struct InputAddDeviceOutput *);
  int (*get_device)(struct InputGetDeviceInput *,
                    struct InputGetDeviceOutput *);

  int (*set_device_callback)(struct InputSetCallbackInput *,
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
static int input_init_intrfc(void) {
  int err;

  log_ops = get_logging_utils_ops();
  input_private_ops = get_input_private_ops();

  log_ops->log_info(INPUT_FILE_NAME, "Initializing input subsystem.");

  err = input_private_ops->init(&input_subsystem);
  if (err) {
    log_ops->log_err(INPUT_FILE_NAME, "Failed to initialize registrar: %s",
                     strerror(err));
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME,
                    "Input subsystem initialized successfully.");

  return 0;
}

static int input_add_device_intrfc(struct InputAddDeviceInput input,
                                   struct InputAddDeviceOutput *output) {
  int err;

  if (!output || !input.device)
    return EINVAL;

  input.private = &input_subsystem;

  err = input_private_ops->add_device(&input, output);
  if (err) {
    log_ops->log_info(INPUT_FILE_NAME, "Module registration failed for: %s.",
                      input.device->display_name);
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME, "Module registered successfully.");

  return 0;
}

static int input_get_device_intrfc(struct InputGetDeviceInput input,
                                   struct InputGetDeviceOutput *output) {
  int err;

  if (!output)
    return EINVAL;

  input.private = &input_subsystem;

  err = input_private_ops->get_device(&input, output);
  if (err) {
    log_ops->log_info(INPUT_FILE_NAME, "Module retrieving failed for: %d.",
                      input.device_id);
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME, "Module retrieved successfully.");

  return 0;
}

static int input_set_callback_intrfc(struct InputSetCallbackInput input,
                                     struct InputSetCallbackOutput *output) {
  int err;

  if (!output || !input.callback)
    return EINVAL;

  log_ops->log_info(INPUT_FILE_NAME, "Setting a registration callback.");

  input.private = &input_subsystem;

  err = input_private_ops->set_device_callback(&input, output);
  if (err) {
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME, "Callback set successfully.");

  return 0;
}

static int input_start_intrfc(void) {
  int err;

  log_ops->log_info(INPUT_FILE_NAME, "Starting input subsystem.");

  err = input_private_ops->start(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME, "Input subsystem started successfully.");
  return 0;
}

static int input_stop_intrfc(void) {
  int err;

  log_ops->log_info(INPUT_FILE_NAME, "Stopping input subsystem.");

  err = input_private_ops->stop(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME, "Input subsystem stopped successfully.");
  return 0;
}

static int input_wait_intrfc(void) {
  int err;

  log_ops->log_info(INPUT_FILE_NAME, "Waiting for input subsystem.");

  err = input_private_ops->wait(&input_subsystem);
  if (err) {
    return err;
  }

  log_ops->log_info(INPUT_FILE_NAME, "Input subsystem waited successfully.");
  return 0;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int input_init(struct InputSubsystem *subsystem) {
  InputSubsystem_devices_init(subsystem);

  return 0;
}

static int input_add_device(struct InputAddDeviceInput *input,
                            struct InputAddDeviceOutput *output) {
  struct InputSubsystem *input_sys;
  int err;

  if (!output || !input->device) {
    return EINVAL;
  }

  output->device_id = -1;
  input->device->callback = NULL;
  input_sys = input->private;

  err = InputSubsystem_devices_append(input_sys, *input->device);
  if (err) {
    return err;
  }

  output->device_id = InputSubsystem_devices_length(input_sys) - 1;

  log_ops->log_info(INPUT_FILE_NAME,
                    "Module registered successfully with ID: %d",
                    output->device_id);

  return 0;
}

static int input_get_device(struct InputGetDeviceInput *input,
                            struct InputGetDeviceOutput *output) {
  struct InputSubsystem *input_sys;
  int err;

  if (!output) {
    return EINVAL;
  }

  output->device = NULL;
  input_sys = input->private;

  err =
      InputSubsystem_devices_get(input_sys, input->device_id, &output->device);
  if (err) {
    return err;
  }

  return 0;
}

static int get_device_by_id(struct InputGetDeviceExtendedInput *input,
                            struct InputGetDeviceExtendedOutput *output) {
  struct InputDevice *device;
  int err;

  err = InputSubsystem_devices_get(&input_subsystem, input->device_id, &device);
  if (err) {
    return err;
  }

  output->device = device;
  output->device_id = input->device_id;
  output->device_name = (char *)device->display_name;

  return 0;
}

static int get_device_by_name(struct InputGetDeviceExtendedInput *input,
                              struct InputGetDeviceExtendedOutput *output) {
  struct InputDevice *device;
  size_t i;

  for (i = 0; i < InputSubsystem_devices_length(&input_subsystem); i++) {
    if (InputSubsystem_devices_get(&input_subsystem, i, &device) == 0 &&
        strcmp(device->display_name, input->device_name) == 0) {
      output->device = device;
      output->device_id = i;
      output->device_name = (char *)device->display_name;
      return 0;
    }
  }

  return ENOENT;
}

static int
input_get_device_extended(struct InputGetDeviceExtendedInput *input,
                          struct InputGetDeviceExtendedOutput *output) {
  int (*get_device_funcs[])(struct InputGetDeviceExtendedInput *,
                            struct InputGetDeviceExtendedOutput *) = {
      [INPUT_GET_DEVICE_BY_ID] = get_device_by_id,
      [INPUT_GET_DEVICE_BY_NAME] = get_device_by_name,
  };

  if (!input || !output || input->mode <= INPUT_GET_DEVICE_NONE ||
      input->mode >= INPUT_GET_DEVICE_INVALID) {
    return EINVAL;
  }

  return get_device_funcs[input->mode](input, output);
}
static int input_set_callback(struct InputSetCallbackInput *input,
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
    log_ops->log_err(INPUT_FILE_NAME, "Failed to retrieve device for ID %d: %s",
                     input->device_id, strerror(err));
    return err;
  }

  device->callback = input->callback;

  log_ops->log_info(INPUT_FILE_NAME, "Callback set successfully for ID %d",
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
      log_ops->log_err(INPUT_FILE_NAME,
                       "Failed to get device for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    // If there is no callback set for device there is no point in starting it.
    if (!device->callback)
      continue;

    err = device->start();
    if (err) {
      log_ops->log_err(INPUT_FILE_NAME, "Failed to start device '%d:%s': %s", i,
                       device->display_name, strerror(err));
      return err;
    }

    log_ops->log_err(INPUT_FILE_NAME, "Started device '%d:%s'", i,
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
      log_ops->log_err(INPUT_FILE_NAME,
                       "Failed to get device for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    if (!device->callback)
      continue;

    err = device->stop();
    if (err) {
      log_ops->log_err(INPUT_FILE_NAME, "Failed to stop device '%d:%s': %s", i,
                       device->display_name, strerror(err));
      return err;
    }

    // Once device is stopped we want to deregister callback.
    // This indicates that device stopped running.
    device->callback = NULL;

    log_ops->log_err(INPUT_FILE_NAME, "Stoped device '%d:%s'", i,
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
      log_ops->log_err(INPUT_FILE_NAME,
                       "Failed to get device for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    if (!device->callback)
      continue;

    err = device->wait();
    if (err) {
      log_ops->log_err(INPUT_FILE_NAME, "Failed to wait for device '%d:%s': %s",
                       i, device->display_name, strerror(err));
      return err;
    }

    log_ops->log_err(INPUT_FILE_NAME, "Waited for device '%d:%s'", i,
                     device->display_name);
  }

  return 0;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputOps input_ops = {.init = input_init_intrfc,
                                    .stop = input_stop_intrfc,
                                    .wait = input_wait_intrfc,
                                    .start = input_start_intrfc,
                                    .add_device = input_add_device_intrfc,
                                    .get_device = input_get_device_intrfc,
                                    .set_callback = input_set_callback_intrfc,
                                    .get_device_extended =
                                        input_get_device_extended};

static struct InputPrivateOps input_private_ops_ = {
    .init = input_init,
    .stop = input_stop,
    .wait = input_wait,
    .start = input_start,
    .add_device = input_add_device,
    .get_device = input_get_device,
    .set_device_callback = input_set_callback,
};

struct InputOps *get_input_ops(void) { return &input_ops; }

struct InputPrivateOps *get_input_private_ops(void) {
  return &input_private_ops_;
}
