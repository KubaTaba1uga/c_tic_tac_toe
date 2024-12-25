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
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

// Data
enum InputEvents {
  INPUT_EVENT_NONE = 0,
  INPUT_EVENT_UP,
  INPUT_EVENT_DOWN,
  INPUT_EVENT_LEFT,
  INPUT_EVENT_RIGHT,
  INPUT_EVENT_SELECT,
  INPUT_EVENT_EXIT,
  INPUT_EVENT_INVALID,
};

typedef int input_device_id_t;
typedef int (*input_wait_func_t)(void);
typedef int (*input_stop_func_t)(void);
typedef int (*input_start_func_t)(void);
typedef int (*input_callback_func_t)(enum InputEvents, input_device_id_t);

struct InputDevice {
  input_wait_func_t wait;
  input_stop_func_t stop;
  input_start_func_t start;
  const char *display_name;
  input_callback_func_t callback;
};

// Ops
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

struct InputOps {
  int (*init)(void);
  void (*destroy)(void);
  int (*start)(void);
  int (*stop)(void);
  int (*wait)(void);
  int (*set_callback)(struct InputSetCallbackInput,
                      struct InputSetCallbackOutput *);
  int (*add_device)(struct InputAddDeviceInput, struct InputAddDeviceOutput *);
};

struct InputOps *get_input_ops(void);

#endif // INPUT_H
