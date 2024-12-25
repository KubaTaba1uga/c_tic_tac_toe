#include "input/keyboard/keyboard_keys_mapping.h"

struct KeyboardKeysMapping1PrivOps {
  keyboard_key_mapping_callback_t keyboard_callback;
};

struct KeyboardKeysMapping1PrivOps *get_keyboard_keys_mapping_1_priv_ops(void);
