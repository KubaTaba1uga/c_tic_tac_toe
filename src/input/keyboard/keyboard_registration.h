/*******************************************************************************
 * @file keyboard_registration.h
 * @brief Header for initializing and destroying KeyboardRegistration structures
 ******************************************************************************/

#ifndef KEYBOARD_REGISTRATION_H
#define KEYBOARD_REGISTRATION_H

#include <stddef.h>

#include "utils/registration_utils.h"

// Data
typedef int (*keyboard_callback_func_t)(size_t n, char buffer[n]);

struct KeyboardRegistrationData {
  keyboard_callback_func_t callback;
};

struct KeyboardRegistration {
  struct KeyboardRegistrationData data;
  struct Registration registration;
};

// Ops
struct KeyboardRegistrationOps {
  int (*init)(struct KeyboardRegistration *, char *, keyboard_callback_func_t);
};

struct KeyboardRegistrationOps *get_keyboard_registration_ops(void);

#endif // KEYBOARD_REGISTRATION_H
