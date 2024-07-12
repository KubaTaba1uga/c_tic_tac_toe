/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// App's internal libs
#include "init/init.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS
 ******************************************************************************/
#define MAX_INIT_REGISTRATIONS 100
static const char module_id[] = "init_subsystem";
struct init_subsystem_t {
  struct init_registration_data registrations[MAX_INIT_REGISTRATIONS];
  size_t count;
};
struct init_subsystem_t init_subsystem = {.count = 0};

static void
init_register_subsystem(struct init_registration_data init_registration_data);
static int initialize_system(void);
static void destroy_system(void);

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct init_ops init_ops = {.register_module = init_register_subsystem,
                            .initialize_system = initialize_system,
                            .destroy_system = destroy_system};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
void init_register_subsystem(
    struct init_registration_data init_registration_data) {
  if (init_subsystem.count < MAX_INIT_REGISTRATIONS) {
    init_subsystem.registrations[init_subsystem.count++] =
        init_registration_data;
  } else {
    logging_utils_ops.log_err(module_id,
                              "Unable to register %s in init, "
                              "no enough space in `registrations` array.",
                              init_registration_data.id);
  }
}

int initialize_system(void) {
  int err = 0;

  size_t i;

  for (i = 0; i < init_subsystem.count; ++i) {

    err = init_subsystem.registrations[i].init_func();
    if (err) {
      logging_utils_ops.log_err(module_id, "Failed to initialize %s: %s",
                                init_subsystem.registrations[i].id,
                                strerror(err));
      return err;
    }

    logging_utils_ops.log_info(module_id, "Initialized %s",
                               init_subsystem.registrations[i].id);
  }

  return 0;
}

void destroy_system() {
  size_t i;

  for (i = 0; i < init_subsystem.count; ++i) {
    logging_utils_ops.log_info(module_id, "Destroying %s",
                               init_subsystem.registrations[i].id);

    init_subsystem.registrations[i].destroy_func();
  }
}
