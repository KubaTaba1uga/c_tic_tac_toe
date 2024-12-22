/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "input/input.h"
#include "input/input_registration.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int input_registration_init(struct InputRegistration *input_reg,
                                   const char *display_name,
                                   input_wait_func_t wait,
                                   input_start_func_t start,
                                   input_stop_func_t stop) {
  struct RegistrationUtilsOps *reg_ops = get_registration_utils_ops();
  struct LoggingUtilsOps *log_ops = get_logging_utils_ops();
  int err;

  if (!input_reg || !display_name || !wait || !start || !stop) {
    return EINVAL;
  }

  err = reg_ops->registration_init(&input_reg->registration,
                                   (char *)display_name, &input_reg->data);
  if (err) {
    log_ops->log_err(__FILE_NAME__, "Unable to init '%s' registration",
                     display_name);
    return ENOMEM;
  }

  input_reg->data.wait = wait;
  input_reg->data.start = start;
  input_reg->data.stop = stop;
  input_reg->data.callback = NULL;

  // registration_init copy the display_name into input_reg->registration
  //  so we need to sync pointers to point to one str.
  input_reg->data.display_name = input_reg->registration.display_name;

  return 0;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputRegistrationOps input_reg_ops = {
    .init = input_registration_init,
};

struct InputRegistrationOps *get_input_reg_ops(void) { return &input_reg_ops; }
