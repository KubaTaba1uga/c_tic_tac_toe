#ifndef REGISTRATION_UTILS_H
#define REGISTRATION_UTILS_H
/*******************************************************************************
 * @file registration_utils.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stdbool.h>
#include <stddef.h>

#include "array_utils.h"

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
// Data
#define REGISTRATION_DISPLAY_NAME_MAX 32

struct Registrar {
  char display_name[REGISTRATION_DISPLAY_NAME_MAX];
  struct Array registrations;
};

struct Registration {
  char display_name[REGISTRATION_DISPLAY_NAME_MAX];
  void *private;
};

// Ops
struct RegisterInput {
  struct Registrar *registrar;
  struct Registration *registration;
};

struct RegisterOutput {
  int registration_id;
};

struct GetRegistrationInput {
  struct Registrar *registrar;
  int registration_id;
};

struct GetRegistrationOutput {
  struct Registration *registration;
};

struct RegistrationUtilsOps {
  int (*init)(struct Registrar *, char *, const size_t);
  void (*destroy)(struct Registrar *);
  int (*registration_init)(struct Registration *, char *, void *);
  int (*register_input_init)(struct RegisterInput *, struct Registrar *,
                             struct Registration *);
  int (*register_module)(struct RegisterInput, struct RegisterOutput *);
  int (*get_registration)(struct GetRegistrationInput,
                          struct GetRegistrationOutput *);
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData init_registration_utils_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct RegistrationUtilsOps *get_registration_utils_ops(void);

#endif // REGISTRATION_UTILS_H
