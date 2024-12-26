#ifndef GAME_USER_H
#define GAME_USER_H

#include "input/input.h"
#include "input/input_common.h"

/**
 * @struct GameUser
 * @brief Represents a user in the game, including their ID, display name, and
 * associated input device.
 */
struct GameUser {
  int user_id;                 /**< Unique identifier for the user */
  const char *display_name;    /**< Display name of the user */
  input_device_id_t device_id; /**< Associated input device ID */
};

/**
 * @struct GameUserOps
 * @brief Operations for initializing and managing GameUser objects.
 */
struct GameUserOps {
  int (*init_user)(struct GameUser *user, int user_id, const char *display_name,
                   input_device_id_t device_id);
  void (*destroy_user)(struct GameUser *user);
};

/**
 * @brief Fetches the operations structure for managing GameUser objects.
 *
 * @return Pointer to the GameUserOps structure.
 */
struct GameUserOps *get_game_user_ops(void);

#endif // GAME_USER_H
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
