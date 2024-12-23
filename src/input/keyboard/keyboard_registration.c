/*******************************************************************************
 * @file keyboard_registration.c
 * @brief Implementation for initializing and destroying KeyboardRegistration
 * structures, triggering callbacks, and retrieving module ID.
 ******************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keyboard_registration.h"
#include "utils/registration_utils.h"

int keyboard_registration_init(struct KeyboardRegistration *registration,
                               char *display_name,
                               keyboard_callback_func_t callback) {
  struct RegistrationUtilsOps *reg_ops = get_registration_utils_ops();
  int err;

  if (!registration || !display_name || !callback) {
    return EINVAL;
  }

  err = reg_ops->registration_init(&registration->registration, display_name,
                                   &registration->data);
  if (err) {
    return err;
  }

  registration->data.callback = callback;

  return 0; // Success
}

// KeyboardRegistrationOps structure
struct KeyboardRegistrationOps keyboard_reg_ops = {
    .init = keyboard_registration_init,
};

// Function to retrieve the KeyboardRegistrationOps instance
struct KeyboardRegistrationOps *get_keyboard_registration_ops(void) {
  return &keyboard_reg_ops;
}
