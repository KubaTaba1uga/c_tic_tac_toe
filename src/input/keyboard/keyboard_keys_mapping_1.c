#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping.h"
#include "utils/logging_utils.h"

#include <string.h>

struct KeyboardKeysMapping1PrivOps {
  keyboard_key_mapping_callback_t callback;
};

static struct KeyboardKeysMapping keyboard_keys_mapping;

static struct KeyboardKeysMapping1PrivOps *priv_ops;
static struct KeyboardKeysMapping1PrivOps *
get_keyboard_keys_mapping_1_priv_ops(void);

static int keyboard_keys_mapping1_init(void) {
  struct KeyboardKeysMappingOps *keyboard_keys_ops;
  struct LoggingUtilsOps *logging_ops;
  struct KeyboardOps *keyboard_ops;
  int err;

  keyboard_keys_ops = get_keyboard_keys_mapping_ops();
  priv_ops = get_keyboard_keys_mapping_1_priv_ops();
  logging_ops = get_logging_utils_ops();
  keyboard_ops = get_keyboard_ops();

  err = keyboard_keys_ops->init_keys_mapping(&keyboard_keys_mapping,
                                             priv_ops->callback,
                                             KEYBOARD_KEYS_MAPPING_1_DISP_NAME);
  if (err) {
    logging_ops->log_err(KEYBOARD_KEYS_MAPPING_1_DISP_NAME,
                         "Keyboard keys mapping initialization failed: %s",
                         strerror(err));
    return err;
  }

  err = keyboard_ops->add_keys_mapping(
      &(const struct KeyboardAddKeysMappingInput){
          .keys_mapping = &keyboard_keys_mapping,
      },
      &(struct KeyboardAddKeysMappingOutput){});
  if (err) {
    logging_ops->log_err(KEYBOARD_KEYS_MAPPING_1_DISP_NAME,
                         "Adding keyboard keys mapping failed: %s",
                         strerror(err));
    return err;
  }

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

  output->input_event = input_event;

  return 0;
}

struct KeyboardKeysMapping1PrivOps keyboard_keys_mapping1_ops_ = {
    .callback = keyboard_keys_mapping1_callback,
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
