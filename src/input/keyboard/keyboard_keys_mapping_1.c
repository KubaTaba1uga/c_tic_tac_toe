#include <errno.h>
#include <string.h>

#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping.h"
#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "utils/logging_utils.h"

struct KeyboardKeysMapping1Subsystem {
  input_device_id_t device_id;
  int keys_mapping_id;
};

struct KeyboardKeysMapping1PrivOps {
  keyboard_key_mapping_callback_t keyboard_callback;
};

static struct InputOps *input_ops;
static struct LoggingUtilsOps *logging_ops;
static struct KeyboardKeysMapping1Subsystem keys_mapping1;

static struct KeyboardKeysMapping1PrivOps *priv_ops;
struct KeyboardKeysMapping1PrivOps *get_keyboard_keys_mapping_1_priv_ops(void);

static int keyboard_keys_mapping1_init(void) {
  struct KeyboardAddKeysMappingOutput add_keys_output;
  struct KeyboardKeysMappingOps *keyboard_keys_ops;
  struct KeyboardKeysMapping keyboard_keys_mapping;
  struct InputAddDeviceOutput add_device_output;
  struct InputDeviceOps *input_device_ops;
  struct KeyboardOps *keyboard_ops;
  struct InputDevice input_device;
  int err;

  keyboard_keys_ops = get_keyboard_keys_mapping_ops();
  priv_ops = get_keyboard_keys_mapping_1_priv_ops();
  logging_ops = get_logging_utils_ops();
  keyboard_ops = get_keyboard_ops();
  input_ops = get_input_ops();
  input_device_ops = get_input_device_ops();

  err = input_device_ops->init_device(&input_device, keyboard_ops->wait,
                                      keyboard_ops->stop, keyboard_ops->start,
                                      KEYBOARD_KEYS_MAPPING_1_DISP_NAME);
  if (err) {
    logging_ops->log_err(KEYBOARD_KEYS_MAPPING_1_DISP_NAME,
                         "Input device initialization failed: %s",
                         strerror(err));
    return err;
  }

  err = input_ops->add_device(
      (struct InputAddDeviceInput){.device = &input_device},
      &add_device_output);
  if (err) {
    logging_ops->log_err(KEYBOARD_KEYS_MAPPING_1_DISP_NAME,
                         "Adding input device failed: %s", strerror(err));
    return err;
  }

  keys_mapping1.device_id = add_device_output.device_id;

  err = keyboard_keys_ops->init_keys_mapping(
      &keyboard_keys_mapping, priv_ops->keyboard_callback,
      keys_mapping1.device_id, KEYBOARD_KEYS_MAPPING_1_DISP_NAME);
  if (err) {
    logging_ops->log_err(KEYBOARD_KEYS_MAPPING_1_DISP_NAME,
                         "Keyboard keys mapping initialization failed: %s",
                         strerror(err));
    return err;
  }

  err = keyboard_ops->add_keys_mapping(
      &(struct KeyboardAddKeysMappingInput){
          .keys_mapping = &keyboard_keys_mapping,
      },
      &add_keys_output);
  if (err) {
    logging_ops->log_err(KEYBOARD_KEYS_MAPPING_1_DISP_NAME,
                         "Adding keyboard keys mapping failed: %s",
                         strerror(err));
    return err;
  }

  keys_mapping1.keys_mapping_id = add_keys_output.keys_mapping_id;

  return 0;
}

static int keyboard_keys_mapping1_callback(
    const struct KeyboardKeysMappingCallbackInput *input,
    struct KeyboardKeysMappingCallbackOutput *output) {
  enum InputEvents input_event;
  size_t i;

  input_event = INPUT_EVENT_NONE;

  for (i = 0; i < input->n; i++) {
    switch (input->buffer[i]) {
    case 'w':
      input_event = INPUT_EVENT_UP;
      break;
    case 's':
      input_event = INPUT_EVENT_DOWN;
      break;
    case 'a':
      input_event = INPUT_EVENT_LEFT;
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

  output->input_event = input_event;

  return 0;
}

struct KeyboardKeysMapping1PrivOps keyboard_keys_mapping1_ops_ = {
    .keyboard_callback = keyboard_keys_mapping1_callback,
};

struct KeyboardKeysMapping1Ops keyboard_keys_mapping1_ops = {
    .init = keyboard_keys_mapping1_init,
};

struct KeyboardKeysMapping1Ops *get_keyboard_keys_mapping_1_ops(void) {
  return &keyboard_keys_mapping1_ops;
}

struct KeyboardKeysMapping1PrivOps *get_keyboard_keys_mapping_1_priv_ops(void) {
  return &keyboard_keys_mapping1_ops_;
}
