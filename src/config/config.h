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
const char *PROJECT_NAME = "c_tic_tac_toe";

struct config_registration_data {
  const char *var_name;
  const char *default_value;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct config_ops {
  int (*register_var)(struct config_registration_data config_registration_data);
  char *(*get_var)(char *);
};

extern struct config_ops config_ops;

#endif // CONFIG_H
