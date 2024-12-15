#ifndef INPUT_REGISTRATION_H
#define INPUT_REGISTRATION_H

#include "input/input_common.h"

typedef struct InputRegistrationData *input_reg_t;

typedef int (*input_wait_func_t)(void);
typedef int (*input_stop_func_t)(void);
typedef int (*input_start_func_t)(void);
typedef int (*input_unregister_callback_func_t)(char *);
typedef int (*input_register_callback_func_t)(char *, input_callback_func_t);

struct InputRegistrationOps {
  int (*wait)(input_reg_t);
  int (*stop)(input_reg_t);
  int (*start)(input_reg_t);
  void (*destroy)(input_reg_t *);
  const char *(*get_id)(input_reg_t);
  input_callback_func_t (*get_callback)(input_reg_t);
  void (*set_callback)(input_reg_t, input_callback_func_t);
  int (*init)(input_reg_t *, const char *, input_wait_func_t,
              input_start_func_t, input_stop_func_t);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InputRegistrationOps *get_input_reg_ops(void);

#endif // INPUT_REGISTRATION_H
