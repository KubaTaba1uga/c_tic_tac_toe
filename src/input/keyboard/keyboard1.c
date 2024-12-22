/*******************************************************************************
 * @file keyboard1.c
 * @brief Implementation of the Keyboard1 module.
 *
 * This module handles keyboard input using the Keyboard1 subsystem. It
 * provides initialization, destruction, and input handling logic.
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "init/init.h"
#include "input/input.h"
#include "input/input_common.h"
#include "input/input_registration.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.h"
#include "input/keyboard/keyboard_registration.h"
#include "utils/logging_utils.h"
#include "utils/terminal_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct Keyboard1System {
  int input_registration_id;
  int keyboard_registration_id;
};

struct Keyboard1PrivateOps {
  int (*start)(void);
  int (*stop)(void);
  int (*wait)(void);
  int (*callback)(size_t n, char buffer[n]);
};

static struct InputOps *input_ops;
static struct KeyboardOps *keyboard_ops;
static struct LoggingUtilsOps *logging_ops;
static keyboard_reg_t keyboard_keyboard1_reg;
static struct Keyboard1System keyboard1_subsystem;
static struct InputRegistrationOps *input_reg_ops;
static struct InputRegistration input_keyboard1_reg;
static struct KeyboardRegistrationOps *keyboard_reg_ops;

struct Keyboard1PrivateOps *keyboard1_private_ops;
struct Keyboard1PrivateOps *get_keyboard1_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int keyboard1_init(void) {
  struct InputRegisterInput reg_input = {.registration = &input_keyboard1_reg};
  struct InputRegisterOutput reg_output;
  int err;

  keyboard1_subsystem.input_registration_id = -1;
  keyboard1_subsystem.keyboard_registration_id = -1;

  input_ops = get_input_ops();
  keyboard_ops = get_keyboard_ops();
  input_reg_ops = get_input_reg_ops();
  logging_ops = get_logging_utils_ops();
  keyboard1_private_ops = get_keyboard1_priv_ops();
  keyboard_reg_ops = get_keyboard_registration_ops();

  // Initialize the input registration
  err = input_reg_ops->init(
      &input_keyboard1_reg, __FILE_NAME__, keyboard1_private_ops->wait,
      keyboard1_private_ops->start, keyboard1_private_ops->stop);
  if (err) {
    logging_ops->log_err(__FILE_NAME__,
                         "Unable to initialize input registration: %s",
                         strerror(err));
    return err;
  }

  // Register module in input
  err = input_ops->register_module(reg_input, &reg_output);
  if (err) {
    logging_ops->log_err(__FILE_NAME__, "Unable to register module: %s",
                         strerror(err));
    return err;
  }

  keyboard1_subsystem.input_registration_id = reg_output.registration_id;

  return 0;
}

static void keyboard1_destroy(void) {
  keyboard_reg_ops->destroy(&keyboard_keyboard1_reg);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int keyboard1_stop(void) {
  keyboard_ops->stop();
  return 0;
}

static int keyboard1_start(void) {
  int err;

  // Register callback for handling input
  err = keyboard_ops->register_callback(keyboard_keyboard1_reg);
  if (err) {
    logging_ops->log_err(
        __FILE_NAME__,
        "Unable to register callback for Keyboard1 subsystem: %s",
        strerror(err));
    return err;
  }

  // Start the keyboard subsystem
  err = keyboard_ops->start();
  if (err) {
    logging_ops->log_err(__FILE_NAME__,
                         "Unable to start Keyboard1 subsystem: %s",
                         strerror(err));
    return err;
  }

  return 0;
}

static int keyboard1_wait(void) {
  keyboard_ops->wait();
  return 0;
}

static int keyboard1_callback(size_t n, char buffer[n]) {
  input_callback_func_t callback;
  enum InputEvents input_event;
  size_t i;
  int err;

  input_event = INPUT_EVENT_NONE;
  callback = input_keyboard1_reg.data.callback;

  if (!callback) {
    logging_ops->log_err(__FILE_NAME__, "No callback set up for Keyboard1");
    return EINVAL;
  }

  for (i = 0; i < n; i++) {
    switch (buffer[i]) {
    case 'w':
      input_event = INPUT_EVENT_UP;
      break;
    case 'a':
      input_event = INPUT_EVENT_LEFT;
      break;
    case 's':
      input_event = INPUT_EVENT_DOWN;
      break;
    case 'd':
      input_event = INPUT_EVENT_RIGHT;
      break;
    case '\n':
      input_event = INPUT_EVENT_SELECT;
      break;
    case 'q':
      input_event = INPUT_EVENT_EXIT;
      break;
    default:
      break;
    }
  }

  err = callback(input_event);
  if (err != 0) {
    logging_ops->log_err(__FILE_NAME__, "Callback failed: %s", strerror(err));
    return err;
  }

  return 0;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistration init_keyboard1_reg = {
    .data = {
        .display_name = INPUT_KEYBOARD1_ID,
        .init = keyboard1_init,
        .destroy = keyboard1_destroy,
    }};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct Keyboard1Ops keyboard1_ops = {};

struct Keyboard1Ops *get_keyboard1_ops(void) { return &keyboard1_ops; }

static struct Keyboard1PrivateOps keyboard1_private_ops_ = {
    .stop = keyboard1_stop,
    .wait = keyboard1_wait,
    .start = keyboard1_start,
    .callback = keyboard1_callback,
};

struct Keyboard1PrivateOps *get_keyboard1_priv_ops(void) {
  return &keyboard1_private_ops_;
}
