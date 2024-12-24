#include "static_array_lib.h"
#include "utils/logging_utils.h"

#define INIT_MAX_MODULES 100

/* InitSubsystem containing static array for module registrations */
typedef struct InitSubsystem {
  SARRS_FIELD(modules, struct InitRegistration, INIT_MAX_MODULES);
} InitSubsystem;

SARRS_DECL(InitSubsystem, modules, struct InitRegistration, INIT_MAX_MODULES);

static struct InitSubsystem init_subsystem;

struct InitPrivateOps {
  int (*register_module)(struct InitRegistration);
};

struct InitPrivateOps *get_init_priv_ops(void);
