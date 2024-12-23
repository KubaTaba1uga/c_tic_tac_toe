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
#include <stddef.h>

#include "input/keyboard/keyboard_registration.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct KeyboardRegisterInput {
  struct KeyboardRegistration *registration;
  void *keyboard;
};

struct KeyboardRegisterOutput {
  int registration_id;
};

struct KeyboardOps {
  int (*wait)(void);
  void (*stop)(void);
  int (*start)(void);
  int (*register_callback)(struct KeyboardRegisterInput,
                           struct KeyboardRegisterOutput *);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct KeyboardOps *get_keyboard_ops(void);

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistration init_keyboard_reg;

#endif // KEYBOARD_H
