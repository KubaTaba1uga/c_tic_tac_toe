#ifndef KEYBOARD_H
#define KEYBOARD_H
/*******************************************************************************
 * @file keyboard.h
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
 *    PUBLIC API
 ******************************************************************************/
typedef int (*keyboard_callback_func_t)(size_t n, char buffer[n]);

struct KeyboardOps {
  int (*initialize)(void);
  void (*destroy)(void);
  void (*wait)(void);
  int (*register_callback)(keyboard_callback_func_t);
  void *(private_ops);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct KeyboardOps *get_keyboard_ops(void);

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData *init_keyboard_reg_p;

#endif // KEYBOARD_H
