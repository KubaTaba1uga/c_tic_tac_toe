#ifndef INPUT_H
#define INPUT_H
/*******************************************************************************
 * @file input.h
 * @brief Input subsystem header.
 *
 * Provides an API for managing input devices and callbacks.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

#include "init/init.h"
#include "input/input_common.h"
#include "input/input_registration.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

// Ops
struct InputSetRegistrationCallbackInput {
  input_callback_func_t callback;
  int registration_id;
  void *input;
};

struct InputSetRegistrationCallbackOutput {};

struct InputRegisterInput {
  struct InputRegistration *registration;
  void *input;
};

struct InputRegisterOutput {
  int registration_id;
};

struct InputOps {
  int (*start)(void);
  int (*stop)(void);
  int (*wait)(void);
  int (*set_callback)(struct InputSetRegistrationCallbackInput,
                      struct InputSetRegistrationCallbackOutput *);
  int (*register_module)(struct InputRegisterInput,
                         struct InputRegisterOutput *);
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistration init_input_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InputOps *get_input_ops(void);

#endif // INPUT_H
