#ifndef KEYBOARD_KEYS_MAPPING_H
#define KEYBOARD_KEYS_MAPPING_H
/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

#include "input/input.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct KeyboardKeysMappingCallbackInput {
  size_t n;
  char *buffer;
};

struct KeyboardKeysMappingCallbackOutput {
  enum InputEvents input_event;
};

typedef int (*keyboard_key_mapping_callback_t)(
    const struct KeyboardKeysMappingCallbackInput *,
    struct KeyboardKeysMappingCallbackOutput *);

struct KeyboardKeysMapping {
  keyboard_key_mapping_callback_t callback;
  input_device_id_t device_id;
  const char *display_name;
};

struct KeyboardKeysMappingOps {
  int (*init_keys_mapping)(struct KeyboardKeysMapping *,
                           keyboard_key_mapping_callback_t callback,
                           const char *display_name);
};

struct KeyboardKeysMappingOps *get_keyboard_keys_mapping_ops(void);

#endif // KEYBOARD_KEYS_MAPPING_H
