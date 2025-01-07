/*******************************************************************************
 * @file input_device.c
 * @brief Implementation of input device initialization and management.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "input/input_common.h"
#include "input/input_device.h"

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
static int init_input_device(struct InputDevice *device,
                             input_wait_func_t wait_func,
                             input_stop_func_t stop_func,
                             input_start_func_t start_func,
                             const char *display_name) {
  if (!device || !display_name || !start_func || !stop_func || !wait_func) {
    return EINVAL;
  }

  device->display_name = display_name;
  device->wait = wait_func;
  device->stop = stop_func;
  device->start = start_func;

  return 0;
}

/*******************************************************************************
 *    PUBLIC OPERATIONS
 ******************************************************************************/
static struct InputDeviceOps input_device_ops = {
    .init_device = init_input_device,
};

struct InputDeviceOps *get_input_device_ops(void) {
  return &input_device_ops;
}
