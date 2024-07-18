/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS
 ******************************************************************************/
#define MAX_INPUT_REGISTRATIONS 10
static const char module_id[] = "input_subsystem";
struct input_subsystem_t {
  struct input_registration_data registrations[MAX_INPUT_REGISTRATIONS];
  size_t count;
};
struct input_subsystem_t input_subsystem = {.count = 0};

static void
input_register_module(struct input_registration_data init_registration_data);
static int input_register_callback(const char *id,
                                   input_callback_func_t callback);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct input_ops input_ops = {
    .register_module = input_register_module,
    .register_callback = input_register_callback,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

void input_register_module(
    struct input_registration_data input_registration_data) {
  if (input_subsystem.count < MAX_INPUT_REGISTRATIONS) {
    input_subsystem.registrations[input_subsystem.count++] =
        input_registration_data;
  } else {
    logging_utils_ops.log_err(module_id,
                              "Unable to register %s in input, "
                              "no enough space in `registrations` array.",
                              input_registration_data.id);
  }
}

int input_register_callback(const char *id, input_callback_func_t callback) {
  size_t i;

  for (i = 0; i < input_subsystem.count; ++i) {
    if (strcmp(id, input_subsystem.registrations[i].id) == 0) {
      input_subsystem.registrations[i].callback = callback;
      return 0;
    }
  }

  logging_utils_ops.log_err(module_id,
                            "Unable to register callback for %s, "
                            "no input module with this id",
                            input_subsystem.registrations[i].id);

  return EINVAL;
}
