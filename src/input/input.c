/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_INPUT_REGISTRATIONS 10
#define INPUT_MODULE_ID "input_subsystem"

struct InputSubsystem {
  struct InputRegistrationData *registrations[MAX_INPUT_REGISTRATIONS];
  size_t count;
};

struct InputSubsystem input_subsystem = {.count = 0};

static int input_init(void);
static void input_destroy(void);
static void
input_register_module(struct InputRegistrationData *init_registration_data);
static int input_register_callback(char *id, input_callback_func_t callback);
static int input_start_non_blocking(void);

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_input_reg = {.id = INPUT_MODULE_ID,
                                                     .init_func = input_init,
                                                     .destroy_func =
                                                         input_destroy,
                                                     .child_count = 0};
struct InitRegistrationData *init_input_reg_p = &init_input_reg;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct InputOps input_ops = {.register_module = input_register_module,
                             .register_callback = input_register_callback,
                             .start = input_start_non_blocking};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
void input_register_module(
    struct InputRegistrationData *input_registration_data) {

  input_registration_data->callback = NULL;

  if (input_subsystem.count < MAX_INPUT_REGISTRATIONS) {
    input_subsystem.registrations[input_subsystem.count++] =
        input_registration_data;
  } else {
    logging_utils_ops.log_err(INPUT_MODULE_ID,
                              "Unable to register %s in input, "
                              "no enough space in `registrations` array.",
                              input_registration_data->id);
  }
}

int input_register_callback(char *id, input_callback_func_t callback) {
  size_t i;

  for (i = 0; i < input_subsystem.count; ++i) {
    if (std_lib_utils_ops.are_str_eq(
            id, (char *)input_subsystem.registrations[i]->id)) {
      input_subsystem.registrations[i]->callback = callback;
      return 0;
    }
  }

  logging_utils_ops.log_err(INPUT_MODULE_ID,
                            "Unable to register callback for %s, "
                            "no input module with this id",
                            input_subsystem.registrations[i]->id);

  return EINVAL;
}

int input_start_non_blocking(void) {
  size_t i;
  int err;

  for (i = 0; i < input_subsystem.count; ++i) {
    if (input_subsystem.registrations[i]->callback == NULL)
      continue;

    err = input_subsystem.registrations[i]->start();

    if (err != 0) {
      logging_utils_ops.log_err(
          INPUT_MODULE_ID, "Unable to start module %s: %s",
          input_subsystem.registrations[i]->id, strerror(err));
      return err;
    }
  }

  return 0;
}

int input_init(void) {
  /* size_t i; */
  /* for (i = 0; i < init_input_reg.child_count; ++i) { */

  /* } */
  return 0;
}

void input_destroy(void) {}

INIT_REGISTER_SUBSYSTEM(init_input_reg_p, INIT_MODULE_ORDER_INPUT);
