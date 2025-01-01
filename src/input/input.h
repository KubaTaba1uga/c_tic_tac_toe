#ifndef INPUT_H
#define INPUT_H
/*******************************************************************************
 * @file input.h
 * @brief Input subsystem header.
 *
 * Provides an API for managing input devices and callbacks.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

#include "init/init.h"
#include "input/input_common.h"
#include "input/input_device.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

#define INPUT_FILE_NAME "input.c"

struct InputSetCallbackInput {
  input_callback_func_t callback;
  input_device_id_t device_id;
  void *private;
};

struct InputSetCallbackOutput {};

struct InputAddDeviceInput {
  struct InputDevice *device;
  void *private;
};

struct InputAddDeviceOutput {
  input_device_id_t device_id;
};

struct InputGetDeviceInput {
  input_device_id_t device_id;
  void *private;
};

struct InputGetDeviceOutput {
  struct InputDevice *device;
};

enum InputGetDeviceMode {
  INPUT_GET_DEVICE_NONE = 0,
  INPUT_GET_DEVICE_BY_ID,
  INPUT_GET_DEVICE_BY_NAME,
  INPUT_GET_DEVICE_INVALID
};

struct InputGetDeviceExtendedInput {
  enum InputGetDeviceMode mode;
  char *device_name;
  input_device_id_t device_id;
  void *private;
};

struct InputGetDeviceExtendedOutput {
  struct InputDevice *device;
  char *device_name;
  input_device_id_t device_id;
};

struct InputOps {
  int (*init)(void);
  void (*destroy)(void);
  int (*start)(void);
  int (*stop)(void);
  int (*wait)(void);
  int (*set_callback)(struct InputSetCallbackInput,
                      struct InputSetCallbackOutput *);
  int (*add_device)(struct InputAddDeviceInput, struct InputAddDeviceOutput *);
  int (*get_device)(struct InputGetDeviceInput, struct InputGetDeviceOutput *);
  int (*get_device_extended)(struct InputGetDeviceExtendedInput *,
                             struct InputGetDeviceExtendedOutput *);
};

struct InputOps *get_input_ops(void);

#endif // INPUT_H
