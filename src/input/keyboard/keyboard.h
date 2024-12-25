#ifndef KEYBOARD_H
#define KEYBOARD_H
/*******************************************************************************
 * @file keyboard.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "input/keyboard/keyboard_keys_mapping.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct KeyboardAddKeysMappingInput {
  struct KeyboardKeysMapping *keys_mapping;
  void *private;
};

struct KeyboardAddKeysMappingOutput {
  int keys_mapping_id;
};

struct KeyboardOps {
  int (*init)(void);
  void (*destroy)(void);
  int (*wait)(void);
  int (*stop)(void);
  int (*start)(void);
  int (*add_keys_mapping)(struct KeyboardAddKeysMappingInput *,
                          struct KeyboardAddKeysMappingOutput *);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct KeyboardOps *get_keyboard_ops(void);

#endif // KEYBOARD_H
