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
// Data
#define PROJECT_NAME "c_tic_tac_toe"

#define CONFIG_VARIABLE_MAX 32

struct ConfigVariable {
  char var_name[CONFIG_VARIABLE_MAX];
  char default_value[CONFIG_VARIABLE_MAX];
};

// Ops
struct ConfigAddVarInput {
  struct ConfigVariable *var;
  void *config;
};

struct ConfigAddVarOutput {
  int var_id;
};

enum ConfigGetVarMode {
  CONFIG_GET_VAR_NONE = 0,
  CONFIG_GET_VAR_BY_ID,
  CONFIG_GET_VAR_BY_NAME,
  CONFIG_GET_VAR_INVALID
};

struct ConfigGetVarInput {
  enum ConfigGetVarMode mode;
  char *var_name;
  int var_id;
  void *config;
};

struct ConfigGetVarOutput {
  char *var_name;
  char *value;
  int var_id;
};

struct ConfigOps {
  int (*init_var)(struct ConfigVariable *, char *, char *);
  int (*add_var)(struct ConfigAddVarInput, struct ConfigAddVarOutput *);
  int (*get_var)(struct ConfigGetVarInput, struct ConfigGetVarOutput *);
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
