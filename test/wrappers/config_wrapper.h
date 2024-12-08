#include "config/config.h"
#include "utils/array_utils.h"

struct ConfigSubsystem {
  array_t registrations;
};

typedef struct ConfigSubsystem *config_sys_t;

struct ConfigPrivateOps {
  int (*init)(config_sys_t *);
  void (*destroy)(config_sys_t *);
  char *(*get_variable)(config_sys_t, char *);
  int (*register_variable)(config_sys_t, struct ConfigRegistrationData *);
};

static const size_t max_registrations = 100;

struct ConfigPrivateOps *get_config_priv_ops(void);
