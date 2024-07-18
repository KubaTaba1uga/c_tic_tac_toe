#ifndef CONFIG_H
#define CONFIG_H
/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
const char *PROJECT_NAME = "c_tic_tac_toe";

struct config_registration_data {
  char var_name[255];
  char default_value[255];
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
