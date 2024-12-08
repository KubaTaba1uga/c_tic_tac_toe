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
#include <asm-generic/errno.h>
#include <errno.h>
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

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct InitSubsystem {
  array_t registrations;
};

typedef struct InitSubsystem *init_t;

struct InitPrivateOps {
  int (*init)(init_t *);
  void (*destroy)(init_t *);
  int (*register_module)(init_t, struct InitRegistrationData *);
  int (*init_modules)(init_t);
  void (*destroy_modules)(init_t);
  int (*init_registration)(struct InitRegistrationData *registration);
  void (*destroy_registration)(struct InitRegistrationData *registration);
};

static init_t init_subsystem;
static struct ArrayUtilsOps *array_ops;
static struct InitPrivateOps *init_ops;
static struct LoggingUtilsOps *logging_ops;
static const size_t max_registrations = 100;
static const char module_id[] = "init_subsystem";

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

  logging_ops = get_logging_utils_ops();
  array_ops = get_array_utils_ops();
  init_ops = get_init_priv_ops();

  err = init_ops->init(&init_subsystem);
  if (err) {
    logging_ops->log_err(module_id, "Unable to initialize init subsystem.");
    return err;
  }

  for (i = 0; i < sizeof(init_modules) / sizeof(struct InitRegistrationData *);
       i++) {
    init_module = init_modules[i];
    err = init_ops->register_module(init_subsystem, init_module);
    if (err) {
      logging_ops->log_err(module_id, "Unable to register init module %s.",
                           init_module->id);
      return err;
    }

    logging_ops->log_info(module_id, "Registered init module %s.",
                          init_module->id);
  }

  err = init_ops->init_modules(init_subsystem);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init modules.", init_module->id);
    return err;
  }

  return 0;
};

static void init_destroy_system(void) {
  if (!init_ops)
    return;

  init_ops->destroy_modules(init_subsystem);
  init_ops->destroy(&init_subsystem);
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static int init_init(init_t *init) {
  init_t tmp_init;
  int err;

  tmp_init = malloc(sizeof(struct InitSubsystem));
  if (!tmp_init) {
    return ENOMEM;
  }

  if (!array_ops) {
    return ENODATA;
  }

  err = array_ops->init(&tmp_init->registrations, max_registrations);
  if (err) {
    return err;
  }

  *init = tmp_init;

  return 0;
};

static void init_destroy(init_t *init) {
  init_t tmp_init = *init;

  if (!array_ops) {
    return;
  }

  array_ops->destroy(&tmp_init->registrations);
  free(tmp_init);

  *init = NULL;
}

static int init_register(init_t init,
                         struct InitRegistrationData *registration_data) {
  int err;

  if (!array_ops || !logging_ops) {
    return ENODATA;
  }

  if (array_ops->get_length(init->registrations) < max_registrations) {
    err = array_ops->append(init->registrations, registration_data);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to register %s in init, "
                           "unkown error.",
                           registration_data->id);
      return err;
    }
  } else {
    logging_ops->log_err(module_id,
                         "Unable to register %s in init, "
                         "no enough space in `registrations` array.",
                         registration_data->id);
    return ENOMEM;
  }

  return 0;
}

static int init_initialize_registrations(init_t init) {
  struct InitRegistrationData *registration_data;
  int err = 0;
  size_t i;

  if (!array_ops || !logging_ops) {
    return ENODATA;
  }

  for (i = 0; i < array_ops->get_length(init->registrations); ++i) {
    registration_data = array_ops->get_element(init->registrations, i);
    if (!registration_data) {
      logging_ops->log_err(module_id, "No module data");
      return EINVAL;
    }

    err = init_ops->init_registration(registration_data);
    if (err) {
      return err;
    }
  }

  logging_ops->log_info(module_id, "Initialized all modules");

  return 0;
}

static void init_destroy_registrations(init_t init) {
  struct InitRegistrationData *registration_data;
  size_t i;

  if (!array_ops || !logging_ops) {
    return;
  }

  i = array_ops->get_length(init->registrations);

  while (--i) {
    registration_data = array_ops->get_element(init->registrations, i);
    if (!registration_data) {
      logging_ops->log_err(module_id,
                           "Unable to destroy NULL registration data");
      return;
    }

    init_ops->destroy_registration(registration_data);
  }
}

static int
init_initialize_registration(struct InitRegistrationData *registration) {
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  int err;

  if (!logging_ops || !registration) {
    return ENODATA;
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
                                      .destroy_system = init_destroy_system};

static struct InitPrivateOps init_priv_ops = {
    .init = init_init,
    .destroy = init_destroy,
    .register_module = init_register,
    .init_modules = init_initialize_registrations,
    .destroy_modules = init_destroy_registrations,
    .init_registration = init_initialize_registration,
    .destroy_registration = init_destroy_registration};

struct InitOps *get_init_ops(void) { return &init_pub_ops; }

struct InitPrivateOps *get_init_priv_ops(void) { return &init_priv_ops; }
