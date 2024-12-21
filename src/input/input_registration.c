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

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int input_registration_init(struct InputRegistration *input_reg,
                                   const char *display_name,
                                   input_wait_func_t wait,
                                   input_start_func_t start,
                                   input_stop_func_t stop) {
  if (!input_reg || !display_name || !wait || !start || !stop) {
    return EINVAL;
  }

  input_reg->data.display_name = display_name;
  input_reg->data.wait = wait;
  input_reg->data.start = start;
  input_reg->data.stop = stop;
  input_reg->data.callback = NULL;

  return 0;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputRegistrationOps input_reg_ops = {
    .init = input_registration_init,
};

struct InputRegistrationOps *get_input_reg_ops(void) { return &input_reg_ops; }
