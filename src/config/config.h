#ifndef CONFIG_H
#define CONFIG_H
/*******************************************************************************
 * @file config.h
 * @brief Header file for the configuration subsystem.
 *
 * This file contains the definitions and declarations for the configuration
 * subsystem, including data structures and function prototypes for registering
 * and retrieving configuration variables.
 *
 ******************************************************************************/
/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#include "utils/registration_utils.h"

// Data
#define PROJECT_NAME "c_tic_tac_toe"

struct ConfigRegistrationData {
  char *var_name;
  char *default_value;
};

struct ConfigRegistration {
  struct ConfigRegistrationData data;
  struct Registration registration;
};

// Ops
struct ConfigRegisterVarInput {
  struct ConfigRegistration *registration;
  void *config;
};

struct ConfigRegisterVarOutput {
  int registration_id;
};

struct ConfigGetVarInput {
  int registration_id;
  void *config;
};

struct ConfigGetVarOutput {
  char *var_name;
  char *value;
};

struct ConfigOps {
  int (*registration_init)(struct ConfigRegistration *, char *, char *);
  int (*register_system_var)(struct ConfigRegisterVarInput,
                             struct ConfigRegisterVarOutput *);
  int (*get_system_var)(struct ConfigGetVarInput, struct ConfigGetVarOutput *);
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistration init_config_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct ConfigOps *get_config_ops(void);

#endif // CONFIG_H
