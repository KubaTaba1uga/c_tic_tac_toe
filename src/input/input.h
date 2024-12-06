#ifndef INPUT_H
#define INPUT_H
/*******************************************************************************
 * @file input.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define INPUT_KEYBOARD1_ID "keyboard1"

enum InputEvents {
  INPUT_EVENT_NONE = 0,
  INPUT_EVENT_UP,
  INPUT_EVENT_DOWN,
  INPUT_EVENT_LEFT,
  INPUT_EVENT_RIGHT,
  INPUT_EVENT_SELECT,
  INPUT_EVENT_EXIT,
  INPUT_EVENT_MAX,
};

typedef int (*input_callback_func_t)(enum InputEvents input_event);
typedef int (*input_start_func_t)(void);
typedef void (*input_wait_func_t)(void);
typedef void (*input_destroy_func_t)(void);
typedef int (*input_init_func_t)(void);

struct InputRegistrationData {
  input_start_func_t start;
  input_wait_func_t wait;
  input_destroy_func_t destroy;
  input_callback_func_t callback;
  const char *id;
};

struct InputOps {
  void (*register_module)(
      struct InputRegistrationData *input_registration_data);
  int (*register_callback)(char *id, input_callback_func_t callback);
  int (*unregister_callback)(char *id);
  input_start_func_t start;
  input_wait_func_t wait;
  input_init_func_t initialize;
  input_destroy_func_t destroy;
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData init_input_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InputOps *get_input_ops(void);

#endif // INPUT_H
