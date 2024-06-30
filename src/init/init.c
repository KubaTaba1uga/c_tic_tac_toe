#include <stddef.h>
#include <string.h>

#include "init/init.h"
#include "utils/logging_utils.h"

#define MAX_INIT_REGISTRATIONS 100
#define INIT_MODULE_ID "init subsystem"

struct init_subsystem_t {
  struct init_registration_data registrations[MAX_INIT_REGISTRATIONS];
  size_t count;
};

struct init_subsystem_t init_subsystem = {.count = 0};

void init_register_subsystem(
    struct init_registration_data init_registration_data) {
  if (init_subsystem.count < MAX_INIT_REGISTRATIONS) {
    init_subsystem.registrations[init_subsystem.count++] =
        init_registration_data;
  } else {
    logging_utils_ops.log_err("Unable to register %s in init, "
                              "no enough space in `registrations` array.",
                              INIT_MODULE_ID, init_registration_data.id);
  }
}

int initialize_system() {
  int err = 0;
  size_t i;
  for (i = 0; i < init_subsystem.count; ++i) {
    err = init_subsystem.registrations[i].init_func();
    if (err) {
      logging_utils_ops.log_err("Failed to initialize %s: %s", INIT_MODULE_ID,
                                init_subsystem.registrations[i].id,
                                strerror(err));
      return err;
    }

    logging_utils_ops.log_info("Initialized %s", INIT_MODULE_ID,
                               init_subsystem.registrations[i].id);
  }

  return 0;
}

void destroy_system() {
  size_t i;
  for (i = 0; i < init_subsystem.count; ++i) {
    init_subsystem.registrations[i].destroy_func();
  }
}
