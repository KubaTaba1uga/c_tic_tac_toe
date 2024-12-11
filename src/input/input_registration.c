#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input/input.h"
#include "input/input_registration.h"
#include "utils/std_lib_utils.h"

// Define the opaque pointer structure
struct InputRegistrationData {
  const char *id;
  input_wait_func_t wait;
  input_stop_func_t stop;
  input_start_func_t start;
  input_callback_func_t callback;
};

int input_reg_init(input_reg_t *input_reg, const char *id,
                   input_wait_func_t wait, input_start_func_t start,
                   input_stop_func_t stop) {
  struct StdLibUtilsOps *std_lib_ops;
  input_reg_t tmp_reg;

  if (!input_reg || !id || !wait || !start || !stop) {
    return EINVAL;
  }

  std_lib_ops = get_std_lib_utils_ops();

  tmp_reg = std_lib_ops->alloc_raw_mem(sizeof(struct InputRegistrationData));
  if (!tmp_reg) {
    return ENOMEM;
  }

  // Initialize the structure fields
  tmp_reg->id = id;
  tmp_reg->wait = wait;
  tmp_reg->start = start;
  tmp_reg->stop = stop;
  tmp_reg->callback = NULL;

  *input_reg = tmp_reg;

  return 0;
}

void input_reg_destroy(input_reg_t *input_reg) {
  if (!input_reg || !*input_reg) {
    return;
  }

  input_reg_t tmp_reg = *input_reg;

  free(tmp_reg);

  *input_reg = NULL;
}

const char *input_reg_get_id(input_reg_t input_reg) {
  if (!input_reg) {
    return NULL;
  }
  return input_reg->id;
}

int input_reg_wait(input_reg_t input_reg) {
  if (!input_reg) {
    return EINVAL;
  }

  return input_reg->wait();
}

int input_reg_start(input_reg_t input_reg) {
  if (!input_reg) {
    return EINVAL;
  }

  return input_reg->start();
}

int input_reg_stop(input_reg_t input_reg) {
  if (!input_reg) {
    return EINVAL;
  }

  return input_reg->stop();
}

input_callback_func_t input_reg_get_callback(input_reg_t input_reg) {
  if (!input_reg) {
    return NULL;
  }
  return input_reg->callback;
}

void input_reg_set_callback(input_reg_t input_reg,
                            input_callback_func_t callback) {
  if (!input_reg) {
    return;
  }

  input_reg->callback = callback;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InputRegistrationOps input_reg_ops = {
    .init = input_reg_init,
    .wait = input_reg_wait,
    .stop = input_reg_stop,
    .start = input_reg_start,
    .get_id = input_reg_get_id,
    .destroy = input_reg_destroy,
    .get_callback = input_reg_get_callback,
    .set_callback = input_reg_set_callback,
};

struct InputRegistrationOps *get_input_reg_ops(void) { return &input_reg_ops; }
