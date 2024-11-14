#include "config/config.h"
#include <stdlib.h>

#define MAX_CONFIG_TEST_REGISTRATIONS 100

struct ConfigSubsystem {
  size_t count;
  struct ConfigRegistrationData registrations[MAX_CONFIG_TEST_REGISTRATIONS];
};

struct ConfigPrivateOps {
  struct ConfigSubsystem *(*get_subsystem)(void);
};
