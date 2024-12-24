#include "init/init.h"

#define INIT_MAX_MODULES 100

struct InitPrivateOps {
  int (*register_module)(struct InitRegistration);
  int (*register_modules)(void);
};

struct InitPrivateOps *get_init_priv_ops(void);
