#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H
/*******************************************************************************
 * @file input_device.h
 * @brief Input device subsystem header.
 *
 * Provides APIs for initializing and managing input devices within the system.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

#include "input/input_common.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct InputDevice {
  input_wait_func_t wait;
  input_stop_func_t stop;
  input_start_func_t start;
  const char *display_name;
  input_callback_func_t callback;
};

struct InputDeviceOps {
  int (*init_device)(struct InputDevice *, input_wait_func_t, input_stop_func_t,
                     input_start_func_t, const char *);
};

struct InputDeviceOps *get_input_device_ops(void);

#endif // INPUT_DEVICE_H
