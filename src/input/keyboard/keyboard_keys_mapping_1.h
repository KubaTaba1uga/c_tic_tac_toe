#ifndef KEYBOARD_KEYS_MAPPING_1_H
#define KEYBOARD_KEYS_MAPPING_1_H

#include "input/keyboard/keyboard_keys_mapping.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define KEYBOARD_KEYS_MAPPING_1_DISP_NAME "wsad"

struct KeyboardKeysMapping1Ops {
  int (*init)(void);
};

struct KeyboardKeysMapping1Ops *get_keyboard_keys_mapping_1_ops(void);

#endif // KEYBOARD_KEYS_MAPPING_1_H
