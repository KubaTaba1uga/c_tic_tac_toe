#include "utils/registration_utils.h"

struct InitSubsystem {
  struct Registrar registrar;
};

struct InitPrivateOps {
  int (*init_registrar)(struct InitSubsystem *);
  void (*destroy_registrar)(struct InitSubsystem *);
  int (*register_modules)(struct InitSubsystem *);
  int (*register_module)(struct InitSubsystem *, struct InitRegistration *);
  int (*init_modules)(struct InitSubsystem *);
  void (*destroy_modules)(struct InitSubsystem *);
};

struct InitPrivateOps *get_init_priv_ops(void);
