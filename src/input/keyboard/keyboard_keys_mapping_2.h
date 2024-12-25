#ifndef KEYBOARD_KEYS_MAPPING_2_H
#define KEYBOARD_KEYS_MAPPING_2_H

#include "input/keyboard/keyboard_keys_mapping.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct KeyboardKeysMapping2Ops {
  int (*init)(void);
};

struct KeyboardKeysMapping2Ops *get_keyboard_keys_mapping_2_ops(void);

#endif // KEYBOARD_KEYS_MAPPING_2_H
