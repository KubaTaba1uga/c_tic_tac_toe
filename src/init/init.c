/*******************************************************************************
 * @file init.c
 * @brief Initialization subsystem using SARR for managing modules.
 ******************************************************************************/
#include "init.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "static_array_lib.h"
#include "utils/logging_utils.h"

#define INIT_MODULES_MAX 100

typedef struct InitSubsystem {
  SARRS_FIELD(modules, struct InitRegistration, INIT_MODULES_MAX);
} InitSubsystem;

SARRS_DECL(InitSubsystem, modules, struct InitRegistration, INIT_MODULES_MAX);

static struct InitSubsystem init_subsystem;
static struct LoggingUtilsOps *log_ops;

// Ops
struct InitPrivateOps {
  int (*register_module)(struct InitRegistration);
  int (*register_modules)(void);
};
static struct InitPrivateOps *init_priv_ops;
struct InitPrivateOps *get_init_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int init_init_system(void) {
  struct InitRegistration *module;
  int err;

  InitSubsystem_modules_init(&init_subsystem);

  log_ops = get_logging_utils_ops();
  init_priv_ops = get_init_priv_ops();

  err = init_priv_ops->register_modules();
  if (err) {
    log_ops->log_err("INIT", "Failed to register modules: %s", strerror(err));
    return err;
  }

  log_ops->log_info("INIT", "Initializing modules");

  /* Initialize all registered modules */
  for (size_t i = 0; i < InitSubsystem_modules_length(&init_subsystem); i++) {
    log_ops->log_info("INIT", "Initializing module: %i", i);
    err = InitSubsystem_modules_get(&init_subsystem, i, &module);
    if (err) {
      log_ops->log_err("INIT", "Failed to get module '%i': %s", i,
                       strerror(err));

      return err;
    }

    if (!module->init) {
      continue;
    }

    err = module->init();
    if (err) {
      log_ops->log_err("INIT", "Failed to initialize module '%s': %s",
                       module->display_name, strerror(err));
      return err;
    }

    log_ops->log_info("INIT", "Initialized module: %s", module->display_name);
  }

  log_ops->log_info("INIT", "All modules initialized successfully.");
  return 0;
}

static void init_destroy_system(void) {
  struct InitRegistration *module;

  log_ops = get_logging_utils_ops();

  /* Destroy all registered modules in reverse order */
  for (size_t i = InitSubsystem_modules_length(&init_subsystem); i > 0; i--) {
    InitSubsystem_modules_get(&init_subsystem, i - 1, &module);
    if (module->destroy) {
      module->destroy();
      log_ops->log_info("INIT", "Destroyed module: %s", module->display_name);
    }
  }

  log_ops->log_info("INIT", "All modules destroyed successfully.");
}

/*******************************************************************************
 *    MODULE REGISTRATION
 ******************************************************************************/
static int init_register_multiple_modules(void) {
  struct InitRegistration *modules[] = {};

  int num_modules = sizeof(modules) / sizeof(struct InitRegistration *);
  int err;

  for (int i = 0; i < num_modules; i++) {
    err = init_priv_ops->register_module(*modules[i]);
    if (err) {
      log_ops->log_err("INIT", "Failed to register module: %s",
                       modules[i]->display_name);
      return err;
    }
  }

  log_ops->log_info("INIT", "Successfully registered all modules.");
  return 0;
}

static int init_register_module(struct InitRegistration module) {
  int err;

  log_ops = get_logging_utils_ops();

  err = InitSubsystem_modules_append(&init_subsystem, module);
  if (err) {
    log_ops->log_err("INIT", "Module registration failed: %s", strerror(err));
    return err;
  }

  log_ops->log_info("INIT", "Registered module: %s", module.display_name);

  return 0;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InitPrivateOps _init_priv_ops = {
    .register_module = init_register_module,
    .register_modules = init_register_multiple_modules,
};

static struct InitOps init_ops = {
    .initialize = init_init_system,
    .destroy = init_destroy_system,
};

struct InitOps *get_init_ops(void) { return &init_ops; }

struct InitPrivateOps *get_init_priv_ops(void) { return &_init_priv_ops; }
