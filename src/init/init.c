/*******************************************************************************
 * @file init.c
 * @brief This file contains the implementation of an initialization subsystem
 *        for managing the registration, initialization, and destruction of
 *        various subsystems within an application.
 *
 * The initialization subsystem allows for the registration of subsystems,
 * initializing them, and subsequently destroying them. It manages the order
 * and ensures proper error handling and logging.
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "display/display.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/sub_state_machines/quit_sm_module.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard1.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"
#include "utils/std_lib_utils.h"
#include "utils/subsystem_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct InitSubsystem {
  subsystem_t subsystem;
};

typedef struct InitSubsystem *init_sys_t;

struct InitPrivateOps {
  int (*init)(init_sys_t *);
  void (*destroy)(init_sys_t *);
  int (*init_modules)(init_sys_t);
  void (*destroy_modules)(init_sys_t);
  int (*init_registration)(struct InitRegistrationData *);
  void (*destroy_registration)(struct InitRegistrationData *);
  int (*register_module)(init_sys_t, struct InitRegistrationData *);
};

static const char module_id[] = "init_subsystem";
static struct SubsystemUtilsOps *subsystem_ops;
static const size_t max_registrations = 100;
static struct LoggingUtilsOps *logging_ops;
static init_sys_t init_subsystem;

static struct InitPrivateOps *init_priv_ops;
static struct InitPrivateOps *get_init_priv_ops(void);

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static int init_initialize_system(void) {
  struct InitRegistrationData *init_modules[] = {
      &init_logging_reg,   &init_config_reg,
      &init_input_reg,     &init_keyboard1_reg,
      &init_keyboard_reg,  &init_display_reg,
      &init_game_sm_reg,   &init_game_sm_sub_reg,
      &init_user_move_reg, &init_quit_state_machine_reg,
      &init_game_reg,
  };
  struct InitRegistrationData *init_module;
  size_t i;
  int err;

  subsystem_ops = get_subsystem_utils_ops();
  logging_ops = get_logging_utils_ops();
  init_priv_ops = get_init_priv_ops();

  // Subsystem utils module is the only one that needs to be initiated
  //     seperatly.
  err = init_subsystem_utils_reg.init();
  if (err) {
    // Logging module has fallback to stdout/stderr if not initialized
    //   that's why we can init it like all others modules.
    logging_ops->log_err(module_id, "Unable to initialize subsystem module: %s",
                         strerror(err));
    return err;
  }

  err = init_priv_ops->init(&init_subsystem);
  if (err) {
    logging_ops->log_err(module_id, "Unable to initialize init subsystem.");
    return err;
  }

  for (i = 0; i < sizeof(init_modules) / sizeof(struct InitRegistrationData *);
       i++) {
    init_module = init_modules[i];
    err = init_priv_ops->register_module(init_subsystem, init_module);
    if (err) {
      logging_ops->log_err(module_id, "Unable to register init module %s.",
                           init_module->id);
      return err;
    }

    logging_ops->log_info(module_id, "Registered init module %s.",
                          init_module->id);
  }

  err = init_priv_ops->init_modules(init_subsystem);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init modules.", init_module->id);
    return err;
  }

  return 0;
};

static void init_destroy_system(void) {
  if (!init_priv_ops)
    return;

  init_priv_ops->destroy_modules(init_subsystem);
  init_priv_ops->destroy(&init_subsystem);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int init_init(init_sys_t *init) {
  init_sys_t tmp_init;
  int err;

  tmp_init = malloc(sizeof(struct InitSubsystem));
  if (!tmp_init) {
    return ENOMEM;
  }

  if (!subsystem_ops) {
    return ENODATA;
  }

  err = subsystem_ops->init(&tmp_init->subsystem, module_id, max_registrations);
  if (err) {
    return err;
  }

  *init = tmp_init;

  return 0;
};

static void init_destroy(init_sys_t *init) {
  init_sys_t tmp_init = *init;

  if (!subsystem_ops) {
    return;
  }

  subsystem_ops->destroy(&tmp_init->subsystem);
  free(tmp_init);

  *init = NULL;
}

static int init_register(init_sys_t init,
                         struct InitRegistrationData *registration_data) {
  int err;

  if (!subsystem_ops || !logging_ops) {
    return ENODATA;
  }

  if (!init || !registration_data || !registration_data->id) {
    return EINVAL;
  }

  err = subsystem_ops->register_module(init->subsystem, registration_data->id,
                                       registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to register %s in init: %s",
                         registration_data->id, strerror(err));
    return err;
  }

  return 0;
}

static bool init_array_search_filter(const char *_, void *__, void *___) {
  return true;
}

static int init_initialize_registrations(init_sys_t init) {
  struct InitRegistrationData *registration_data = NULL;
  module_search_t search_wrap;
  int err;

  if (!subsystem_ops || !logging_ops) {
    return ENODATA;
  }

  err = subsystem_ops->init_search_module_wrapper(&search_wrap, NULL,
                                                  init_array_search_filter,
                                                  (void **)&registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  do {
    err = subsystem_ops->search_modules(init->subsystem, search_wrap);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to search for"
                           "%s: %s",
                           registration_data->id, strerror(err));
      goto cleanup;
    }
    if (!registration_data)
      break;

    err = init_priv_ops->init_registration(registration_data);
    if (err) {
      goto cleanup;
    }
  } while (true);

  err = 0;

  logging_ops->log_info(module_id, "Initialized all modules");

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return err;
}

static void init_destroy_registrations(init_sys_t init) {
  struct InitRegistrationData *registration_data = NULL;
  module_search_t search_wrap;
  int err;

  if (!subsystem_ops || !logging_ops) {
    return;
  }

  err = subsystem_ops->init_search_module_wrapper(&search_wrap, NULL,
                                                  init_array_search_filter,
                                                  (void **)&registration_data);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init search wrapper: %s",
                         strerror(err));
    goto out;
  }

  // Traverse backwords
  subsystem_ops->set_step_search_module_wrapper(search_wrap, -1);

  do {
    err = subsystem_ops->search_modules(init->subsystem, search_wrap);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to search for"
                           "%s: %s",
                           registration_data->id, strerror(err));
      goto cleanup;
    }
    if (!registration_data)
      break;

    init_priv_ops->destroy_registration(registration_data);
    if (err) {
      goto cleanup;
    }
  } while (true);

  err = 0;

  logging_ops->log_info(module_id, "Destroyed all modules");

cleanup:
  subsystem_ops->destroy_search_module_wrapper(&search_wrap);
out:
  return;
}

static int
init_initialize_registration(struct InitRegistrationData *registration) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  int err;

  if (!logging_ops) {
    return ENODATA;
  }

  if (!registration) {
    return EINVAL;
  }

  if (!registration->init)
    return 0;

  err = registration->init();
  if (err) {
    logging_ops->log_err(module_id, "Failed to initialize %s: %s",
                         registration->id, strerror(err));
    return err;
  }

  logging_ops->log_info(module_id, "Initialized %s", registration->id);

  return 0;
}

static void
init_destroy_registration(struct InitRegistrationData *registration) {
  if (!logging_ops || !registration || !registration->destroy)
    return;

  logging_ops->log_info(module_id, "Destroying %s", registration->id);

  registration->destroy();
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct InitOps init_pub_ops = {.initialize_system =
                                          init_initialize_system,
                                      .destroy_system = init_destroy_system

};

static struct InitPrivateOps init_priv_ops_ = {
    .init = init_init,
    .destroy = init_destroy,
    .register_module = init_register,
    .init_modules = init_initialize_registrations,
    .destroy_modules = init_destroy_registrations,
    .init_registration = init_initialize_registration,
    .destroy_registration = init_destroy_registration};

struct InitOps *get_init_ops(void) { return &init_pub_ops; }

struct InitPrivateOps *get_init_priv_ops(void) { return &init_priv_ops_; }
