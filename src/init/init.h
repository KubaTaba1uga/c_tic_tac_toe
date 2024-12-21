#ifndef INIT_H
#define INIT_H
/*******************************************************************************
 * @file init.h
 * @brief Header file for the initialization subsystem.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "utils/registration_utils.h"

/*******************************************************************************
 *    API
 ******************************************************************************/
struct InitRegistrationData {
  int (*init)(void);
  void (*destroy)(void);
  char *display_name;
};

struct InitRegistration {
  struct InitRegistrationData data;
  struct Registration registration;
};

struct InitOps {
  int (*initialize)(void);
  void (*destroy)(void);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct InitOps *get_init_ops(void);

#endif // INIT_H
