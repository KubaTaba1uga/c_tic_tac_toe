/*******************************************************************************
 * @file keyboard_registration.h
 * @brief Header for initializing and destroying KeyboardRegistration structures
 ******************************************************************************/

#ifndef KEYBOARD_REGISTRATION_H
#define KEYBOARD_REGISTRATION_H

#include <stddef.h>
typedef struct KeyboardRegistration *keyboard_reg_t;

struct KeyboardRegistrationOps {
  int (*init)(keyboard_reg_t *, const char *, int (*)(size_t, char[]));
  void (*destroy)(keyboard_reg_t *);
  int (*callback)(keyboard_reg_t, size_t, char[]);
  const char *(*get_module_id)(keyboard_reg_t);
};

struct KeyboardRegistrationOps *get_keyboard_registration_ops(void);

#endif // KEYBOARD_REGISTRATION_H
