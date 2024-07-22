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

enum input_modules {
  INPUT_KEYBOARD1 = 0,
  INPUT_KEYBOARD2,
  INPUT_MAX,
};

typedef int (*input_callback_func_t)(void);
typedef int (*input_start_func_t)(void);

struct input_registration_data {
  input_start_func_t start;
  input_callback_func_t callback;
  enum input_modules id;
};

struct input_ops {
  void (*register_module)(
      struct input_registration_data input_registration_data);
  int (*register_callback)(enum input_modules, input_callback_func_t callback);
  int (*start)(void);
  void *private_data[INPUT_MAX];
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct init_registration_data *init_input_reg_p;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct input_ops input_ops;

#endif // INPUT_H

/* #define INPUT_REGISTER_MODULE(_id, _start) \ */
/*   static struct input_registration_data _input_registration_data = { \ */
/*       .id = _id, .start = _start}; \ */
/*   input_ops.private_data[_input_] =                        \ */
/*       (void *)&_input_registration_data; */
