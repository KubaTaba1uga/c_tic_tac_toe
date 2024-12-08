#include "utils/array_utils.h"

struct ConfigSubsystem {
  array_t registrations;
};

typedef struct ConfigSubsystem *config_sys_t;

struct ConfigPrivateOps {
  int (*init)(config_sys_t *);
  void (*destroy)(config_sys_t *);
};

static const size_t max_registrations = 100;
