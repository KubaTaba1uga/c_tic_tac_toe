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

// Structure for keyboard registration
struct KeyboardRegistration {
  const char *module_id; // ID of the module registering the callback
  int (*callback)(size_t buffer_size,
                  char buffer[buffer_size]); // Callback function
};

// Function to initialize a KeyboardRegistration structure
int keyboard_registration_init(keyboard_reg_t *registration,
                               const char *module_id,
                               int (*callback)(size_t, char[])) {
  keyboard_reg_t tmp_reg;

  if (!registration || !module_id || !callback) {
    return EINVAL; // Invalid arguments
  }

  // Allocate memory for the registration structure
  tmp_reg = malloc(sizeof(struct KeyboardRegistration));
  if (!tmp_reg) {
    return ENOMEM; // Memory allocation failure
  }

  // Initialize the structure fields
  tmp_reg->module_id = module_id; // Store the module ID
  tmp_reg->callback = callback;   // Assign the callback function

  *registration = tmp_reg;

  return 0; // Success
}

// Function to destroy a KeyboardRegistration structure
void keyboard_registration_destroy(keyboard_reg_t *registration) {
  if (!registration || !*registration) {
    return;
  }

  free(*registration);

  *registration = NULL;
}

// Function to trigger the registered callback
int keyboard_registration_trigger_callback(keyboard_reg_t registration,
                                           size_t buffer_size,
                                           char buffer[buffer_size]) {
  if (!registration || !registration->callback) {
    return EINVAL; // Invalid arguments
  }

  // Call the registered callback function
  return registration->callback(buffer_size, buffer);
}

// Function to retrieve the module ID
const char *keyboard_registration_get_module_id(keyboard_reg_t registration) {
  if (!registration) {
    return NULL; // Invalid argument
  }

  return registration->module_id;
}

// KeyboardRegistrationOps structure
struct KeyboardRegistrationOps keyboard_reg_ops = {
    .init = keyboard_registration_init,
    .destroy = keyboard_registration_destroy,
    .callback = keyboard_registration_trigger_callback,
    .get_module_id = keyboard_registration_get_module_id};

// Function to retrieve the KeyboardRegistrationOps instance
struct KeyboardRegistrationOps *get_keyboard_registration_ops(void) {
  return &keyboard_reg_ops;
}
