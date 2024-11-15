/*******************************************************************************
 * @file keyboard1.c
 * @brief TO-DO
 *
 * TO-DO
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
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static const char *module_id = INPUT_KEYBOARD1_ID;

static int keyboard1_module_init(void);
static void keyboard1_module_destroy(void);
static int keyboard1_start(void);
static void keyboard1_wait(void);
static int keyboard1_callback(size_t n, char buffer[n]);
static void keyboard1_destroy(void);

static struct InputOps *input_ops;
static struct KeyboardOps *keyboard_ops;
static struct LoggingUtilsOps *logging_ops;
struct InputRegistrationData input_keyboard1_reg = {
    .destroy = keyboard1_destroy,
    .start = keyboard1_start,
    .wait = keyboard1_wait,
    .id = INPUT_KEYBOARD1_ID,
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct Keyboard1PrivateOps {
  int (*init)(void);
  void (*destroy)(void);
  int (*start)(void);
  void (*wait)(void);
  int (*callback)(size_t n, char buffer[n]);
};

static struct Keyboard1PrivateOps keyboard1_private_ops = {
    .destroy = keyboard1_destroy,
    .start = keyboard1_start,
    .wait = keyboard1_wait,
    .callback = keyboard1_callback,
};

static struct Keyboard1Ops keyboard1_ops = {.private_ops =
                                                &keyboard1_private_ops};

struct Keyboard1Ops *get_keyboard1_ops(void) { return &keyboard1_ops; };

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_keyboard1_reg = {
    .id = INPUT_KEYBOARD1_ID,
    .init_func = keyboard1_module_init,
    .destroy_func = keyboard1_module_destroy,
};
struct InitRegistrationData *init_keyboard1_reg_p = &init_keyboard1_reg;

/*******************************************************************************
 *    API
 ******************************************************************************/
int keyboard1_module_init(void) {
  input_ops = get_input_ops();
  keyboard_ops = get_keyboard_ops();
  logging_ops = get_logging_utils_ops();

  input_ops->register_module(&input_keyboard1_reg);

  return 0;
}

void keyboard1_module_destroy(void) {}

int keyboard1_start(void) {
  int err;

  err = keyboard_ops->initialize();
  if (err) {
    logging_ops->log_err(module_id, "Unable to initialize keyboard1 module");
    return err;
  }

  // Keyboard callback is set to start gathering and processing input by
  // keyboard.
  err = keyboard_ops->register_callback(keyboard1_private_ops.callback);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to register callback for keyboard1 module");

    return err;
  }

  logging_ops->log_info(module_id, "Keyboard1 started");

  return 0;
}

void keyboard1_wait(void) { keyboard_ops->wait(); }

void keyboard1_destroy(void) { keyboard_ops->destroy(); }

int keyboard1_callback(size_t n, char buffer[n]) {
  enum InputEvents input_event = INPUT_EVENT_NONE;
  size_t i;
  int err;

  // Input callback is set by a game, once user choose it's input device.
  if (input_keyboard1_reg.callback == NULL) {
    logging_ops->log_err(module_id, "No callback set up for keyboard1");
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

    default:;
    }
  }

  err = input_keyboard1_reg.callback(input_event);
  if (err != 0) {
    logging_ops->log_err(module_id, "Callback failed: %s", strerror(err));
    return err;
  }

  return 0;
}

/* INIT_REGISTER_SUBSYSTEM_CHILD(&init_keyboard1_reg, init_keyboard_reg_p); */
INIT_REGISTER_SUBSYSTEM_CHILD(&init_keyboard1_reg, init_input_reg_p);
