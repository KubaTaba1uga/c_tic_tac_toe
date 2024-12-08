#include "utils/array_utils.h"

struct ConfigSubsystem {
  array_t registrations;
};

typedef struct ConfigSubsystem *config_t;

struct ConfigPrivateOps {
  struct ConfigSubsystem *(*get_subsystem)(void);
};

const size_t max_registrations = 10;
