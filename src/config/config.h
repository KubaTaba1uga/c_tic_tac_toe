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
#define PROJECT_NAME "c_tic_tac_toe"

#define CONFIG_VAR_NAME_MAX 100
#define CONFIG_DEFAULT_VALUE_MAX 100

struct ConfigRegistrationData {
  char var_name[CONFIG_VAR_NAME_MAX];
  char default_value[CONFIG_DEFAULT_VALUE_MAX];
};

struct ConfigOps {
  int (*init)(struct ConfigRegistrationData *, char *, char *);
  int (*register_system_var)(struct ConfigRegistrationData *);
  char *(*get_system_var)(char *);
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
extern struct InitRegistrationData init_config_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct ConfigOps *get_config_ops(void);

#endif // CONFIG_H
