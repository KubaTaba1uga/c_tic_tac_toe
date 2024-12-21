#ifndef INPUT_REGISTRATION_H
#define INPUT_REGISTRATION_H

#include "input/input_common.h"
#include "utils/registration_utils.h"

// Data
typedef int (*input_wait_func_t)(void);
typedef int (*input_stop_func_t)(void);
typedef int (*input_start_func_t)(void);

struct InputRegistrationData {
  input_wait_func_t wait;
  input_stop_func_t stop;
  input_start_func_t start;
  const char *display_name;
  input_callback_func_t callback;
};

struct InputRegistration {
  struct InputRegistrationData data;
  struct Registration registration;
};

// Ops

struct InputRegistrationOps {
  int (*init)(struct InputRegistration *, const char *, input_wait_func_t,
              input_start_func_t, input_stop_func_t);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InputRegistrationOps *get_input_reg_ops(void);

#endif // INPUT_REGISTRATION_H
