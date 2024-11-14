#include "config/config.h"
#include <stdlib.h>

#define MAX_CONFIG_REGISTRATIONS 100

struct ConfigSubsystem {
  struct ConfigRegistrationData registrations[MAX_CONFIG_REGISTRATIONS];
  size_t count;
};

struct ConfigPrivateOps {
  struct ConfigSubsystem *(*get_subsystem)(void);
};
