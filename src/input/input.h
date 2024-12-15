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

#include "init/init.h"
#include "input/input_common.h"
#include "input/input_registration.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define INPUT_KEYBOARD1_ID "keyboard1"
struct InputOps {
  int (*wait)(void);
  int (*stop)(void);
  int (*start)(void);
  int (*unregister_callback)(char *);
  int (*register_module)(input_reg_t);
  int (*register_callback)(char *, input_callback_func_t);
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
