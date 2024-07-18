#ifndef INPUT_SUBSYSTEM_H
#define INPUT_SUBSYSTEM_H

#include <stddef.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define INPUT_KEYBOARD1_ID "keyboard1"
#define INPUT_KEYBOARD2_ID "keyboard2"

typedef int (*input_callback_func_t)(void);
typedef int (*input_process_func_t)(void);

struct input_registration_data {
  input_process_func_t process;
  input_callback_func_t callback;
  const char *id;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct input_ops {
  void (*register_module)(
      struct input_registration_data input_registration_data);
  int (*register_callback)(const char *id, input_callback_func_t callback);
};

extern struct input_ops input_ops;

#endif // INPUT_SUBSYSTEM_H
