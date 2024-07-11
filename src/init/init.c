#include <stddef.h>
#include <stdio.h>
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
  printf("\ninit_register_subsystem %s\n", init_registration_data.id);
  if (init_subsystem.count < MAX_INIT_REGISTRATIONS) {
    init_subsystem.registrations[init_subsystem.count++] =
        init_registration_data;
  } else {
    logging_utils_ops.log_err(INIT_MODULE_ID,
                              "Unable to register %s in init, "
                              "no enough space in `registrations` array.",
                              init_registration_data.id);
  }
}

int initialize_system() {
  int err = 0;
  size_t i;

  printf("\ninitialize_system\n");
  for (i = 0; i < init_subsystem.count; ++i) {

    printf("\ninitialize_system %s\n", init_subsystem.registrations[i].id);

    err = init_subsystem.registrations[i].init_func();
    if (err) {
      logging_utils_ops.log_err(INIT_MODULE_ID, "Failed to initialize %s: %s",
                                init_subsystem.registrations[i].id,
                                strerror(err));
      return err;
    }
    logging_utils_ops.log_info("main", "Game initialized");
    // Something is wrong with filling vargs, look on this function and on
    // function below. The only difference between them is vargs.

    logging_utils_ops.log_info(INIT_MODULE_ID, "Initialized %s",
                               init_subsystem.registrations[i].id);
  }

  logging_utils_ops.log_info(INIT_MODULE_ID, "Initialization done",
                             init_subsystem.registrations[i].id);

  return 0;
}

void destroy_system() {
  size_t i;
  for (i = 0; i < init_subsystem.count; ++i) {
    init_subsystem.registrations[i].destroy_func();
  }
}
