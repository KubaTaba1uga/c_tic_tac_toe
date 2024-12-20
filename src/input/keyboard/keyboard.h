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

struct KeyboardRegistration {
  keyboard_callback_func_t callback;
  const char *module_id;
};

struct KeyboardOps {
  int (*wait)(void);
  void (*stop)(void);
  int (*start)(void);
  int (*register_callback)(struct KeyboardRegistration *);
};

extern struct InitRegistrationData init_keyboard_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct KeyboardOps *get_keyboard_ops(void);

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData *init_keyboard_reg_p;

#endif // KEYBOARD_H
