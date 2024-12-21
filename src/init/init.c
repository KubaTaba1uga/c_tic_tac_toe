/*******************************************************************************
 * @file init.c
 * @brief Initialization subsystem for managing the setup and teardown of
 *        various application modules.
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Internal libraries
#include "config/config.h"
#include "init/init.h"
#include "utils/logging_utils.h"
#include "utils/registration_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_REGISTRATIONS 100

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

static struct RegistrationUtilsOps *reg_ops;
static struct InitSubsystem init_subsystem;
static struct LoggingUtilsOps *log_ops;

static struct InitPrivateOps *init_priv_ops;
struct InitPrivateOps *get_init_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int init_system(void) {
  int err;

  reg_ops = get_registration_utils_ops();
  init_priv_ops = get_init_priv_ops();
  log_ops = get_logging_utils_ops();

  err = init_priv_ops->init_registrar(&init_subsystem);

  if (err) {
    log_ops->log_err(__FILE__, "Unable to initialize registrar: %s",
                     strerror(err));

    return err;
  }

  err = init_priv_ops->register_modules(&init_subsystem);
  if (err) {
    log_ops->log_err(__FILE__, "Unable to register modules: %s", strerror(err));

    return err;
  }

  err = init_priv_ops->init_modules(&init_subsystem);
  if (err) {
    log_ops->log_err(__FILE__, "Unable to init modules: %s", strerror(err));
    return err;
  }

  log_ops->log_info(__FILE__, "Init finished");

  return 0;
}

void destroy_system(void) {
  if (init_subsystem.registrar.registrations.data) {
    init_priv_ops->destroy_modules(&init_subsystem);
    init_priv_ops->destroy_registrar(&init_subsystem);
  }

  log_ops->log_info(__FILE_NAME__, "Destroy finished");
}

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/
static int init_initialize_registrar(struct InitSubsystem *subsystem) {
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  err = init_registration_utils_reg.init();
  if (err) {
    // Logging module has fallback to stdout/stderr if not initialized
    //   that's why we can init it like all others modules.
    log_ops->log_err(__FILE__, "Unable to initialize registration module: %s",
                     strerror(err));
    return err;
  }

  err = reg_ops->init(&subsystem->registrar, __FILE__, MAX_REGISTRATIONS);
  if (err) {
    log_ops->log_err(__FILE__, "Failed to initialize registrar: %s",
                     strerror(err));
    return err;
  }

  return 0;
}

static void init_destroy_registrar(struct InitSubsystem *subsystem) {
  if (subsystem) {
    reg_ops->destroy(&subsystem->registrar);
  }
}

static int init_register_modules(struct InitSubsystem *subsystem) {
  struct InitRegistration *init_registrations[] = {
      &init_config_reg,
  };
  size_t i;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  for (i = 0;
       i < sizeof(init_registrations) / sizeof(struct InitRegistration *);
       i++) {

    err = init_priv_ops->register_module(subsystem, init_registrations[i]);
    if (err) {
      log_ops->log_err(__FILE__, "Failed to register module '%s': %s",
                       init_registrations[i]->data.display_name, strerror(err));

      return err;
    }
  }

  return 0;
}

static int init_register_module(struct InitSubsystem *subsystem,
                                struct InitRegistration *module) {
  struct RegisterOutput output;
  struct RegisterInput input;
  int err;

  if (!subsystem || !module) {
    return EINVAL;
  }

  err = reg_ops->registration_init(&module->registration,
                                   module->data.display_name, module);
  if (err) {
    log_ops->log_err(__FILE__, "Failed to initialize module registration: %s",
                     strerror(err));
    return err;
  }

  module->data.display_name = module->registration.display_name;

  reg_ops->register_input_init(&input, &subsystem->registrar,
                               &module->registration);

  err = reg_ops->register_module(input, &output);
  if (err) {
    log_ops->log_err(__FILE__, "Failed to register module: %s", strerror(err));
    return err;
  }

  log_ops->log_info(__FILE__, "Module '%s' registered successfully.",
                    module->data.display_name);
  return 0;
}

static int init_modules(struct InitSubsystem *subsystem) {
  struct GetRegistrationInput reg_input;
  struct GetRegistrationOutput reg_output;
  int err;

  if (!subsystem) {
    return EINVAL;
  }

  reg_input.registrar = &subsystem->registrar;

  for (int i = 0; i < subsystem->registrar.registrations.length; i++) {
    reg_input.registration_id = i;

    err = reg_ops->get_registration(reg_input, &reg_output);
    if (err) {
      log_ops->log_err(__FILE__,
                       "Failed to get registration for module ID %d: %s", i,
                       strerror(err));
      return err;
    }

    struct InitRegistrationData *module = reg_output.registration->private;
    if (module->init) {
      err = module->init();
      if (err) {
        log_ops->log_err(__FILE__, "Failed to initialize module '%s': %s",
                         module->display_name, strerror(err));
        return err;
      }

      log_ops->log_info(__FILE__, "Module '%s' initialized successfully.",
                        module->display_name);
    }
  }

  return 0;
}

static void destroy_modules(struct InitSubsystem *subsystem) {
  struct GetRegistrationInput reg_input;
  struct GetRegistrationOutput reg_output;

  if (!subsystem) {
    return;
  }

  for (int i = subsystem->registrar.registrations.length - 1; i >= 0; i--) {
    reg_input.registrar = &subsystem->registrar;
    reg_input.registration_id = i;

    if (reg_ops->get_registration(reg_input, &reg_output) == 0) {
      struct InitRegistrationData *module = reg_output.registration->private;
      if (module->destroy) {
        module->destroy();
        log_ops->log_info(__FILE__, "Module '%s' destroyed successfully.",
                          module->display_name);
      }
    }
  }
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InitPrivateOps init_priv_ops_ = {
    .init_registrar = init_initialize_registrar,
    .destroy_registrar = init_destroy_registrar,
    .register_modules = init_register_modules,
    .register_module = init_register_module,
    .destroy_modules = destroy_modules,
    .init_modules = init_modules,
};

static struct InitOps init_ops = {
    .initialize = init_system,
    .destroy = destroy_system,
};

struct InitOps *get_init_ops(void) { return &init_ops; }

struct InitPrivateOps *get_init_priv_ops(void) { return &init_priv_ops_; }
