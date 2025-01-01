/*******************************************************************************
 * @file init.c
 * @brief Initialization subsystem using SARR for managing modules.
 ******************************************************************************/
#include "init.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "config/config.h"
#include "display/cli.h"
#include "display/display.h"
#include "game/game.h"
#include "game/game_config.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/display_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/moves_cleanup_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/quit_mini_machine.h"
#include "game/game_state_machine/mini_state_machines/user_move_mini_machine.h"
#include "input/input.h"
#include "input/keyboard/keyboard.h"
#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "static_array_lib.h"
#include "utils/logging_utils.h"
#include "utils/signals_utils.h"

#define INIT_MODULES_MAX 100

struct InitRegistration {
  int (*init)(void);
  void (*destroy)(void);
  const char *display_name;
};

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
static int init_init(void) {
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
    InitSubsystem_modules_get(&init_subsystem, i, &module);

    if (!module->init) {
      continue;
    }

    log_ops->log_info("INIT", "Initializing module: %s", module->display_name);

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

static void init_destroy(void) {
  struct InitRegistration *module;

  log_ops = get_logging_utils_ops();

  /* Destroy all registered modules in reverse order */
  for (size_t i = InitSubsystem_modules_length(&init_subsystem); i > 0; i--) {
    InitSubsystem_modules_get(&init_subsystem, i - 1, &module);
    if (module->destroy) {
      log_ops->log_info("INIT", "Destroying module: %s", module->display_name);
      module->destroy();
    }
  }
}

/*******************************************************************************
 *    MODULE REGISTRATION
 ******************************************************************************/
static int init_register_multiple_modules(void) {
  struct KeyboardKeysMapping1Ops *km1_ops = get_keyboard_keys_mapping_1_ops();
  struct SignalUtilsOps *signals_ops = get_signal_utils_ops();
  struct GameSmSubsystemOps *game_sm_sub_ops = get_game_sm_subsystem_ops();
  struct LoggingUtilsOps *logging_ops = get_logging_utils_ops();
  struct GameConfigOps *game_config_ops = get_game_config_ops();
  struct KeyboardOps *keyboard_ops = get_keyboard_ops();
  struct DisplayCliOps *display_cli_ops = get_display_cli_ops();
  struct GameSmCleanLastMoveModuleOps *clean_last_move_ops =
      get_game_sm_clean_last_move_module_ops();
  struct GameStateMachineOps *game_state_machine_ops =
      get_game_state_machine_ops();
  struct GameSmUserMoveModuleOps *gsm_user_move_ops =
      get_game_sm_user_move_module_ops();
  struct GameSmDisplayModuleOps *gsm_display_ops =
      get_game_sm_display_module_ops();
  struct GameSmQuitModuleOps *gsm_quit_ops = get_game_sm_quit_module_ops();
  struct ConfigOps *config_ops = get_config_ops();
  struct InputOps *input_ops = get_input_ops();
  struct GameOps *game_ops = get_game_ops();
  struct DisplayOps *display_ops = get_display_ops();
  struct InitRegistration modules[] = {
      {.init = logging_ops->init,
       .destroy = logging_ops->destroy,
       .display_name = "logging_utils"},
      {.init = signals_ops->init,
       .destroy = NULL,
       .display_name = "signals_utils"},
      {.init = config_ops->init, .destroy = NULL, .display_name = "config"},
      {.init = input_ops->init, .destroy = NULL, .display_name = "input"},
      {.init = keyboard_ops->init,
       .destroy = keyboard_ops->destroy,
       .display_name = "keyboard"},
      {.init = km1_ops->init,
       .destroy = NULL,
       .display_name = KEYBOARD_KEYS_MAPPING_1_DISP_NAME},
      {.init = display_ops->init, .destroy = NULL, .display_name = "display"},
      {.init = display_cli_ops->init,
       .destroy = NULL,
       .display_name = "display_cli"},
      {.init = game_ops->init, .destroy = NULL, .display_name = "game"},
      {.init = game_config_ops->init,
       .destroy = NULL,
       .display_name = "game_config"},
      {.init = game_state_machine_ops->init,
       .destroy = NULL,
       .display_name = "game_state_machine"},
      {.init = game_sm_sub_ops->init,
       .destroy = NULL,
       .display_name = "game_state_machine_subsystem"},
      {.init = gsm_user_move_ops->init,
       .destroy = NULL,
       .display_name = "user_move_mini_machine"},
      {.init = gsm_quit_ops->init,
       .destroy = NULL,
       .display_name = "quit_mini_machine"},
      {.init = clean_last_move_ops->init,
       .destroy = NULL,
       .display_name = "last_move_cleanup_mini_machine"},
      {.init = gsm_display_ops->init,
       .destroy = NULL,
       .display_name = "display_mini_machine"},

  };
  int num_modules = sizeof(modules) / sizeof(struct InitRegistration);
  int err;
  int i;

  for (i = 0; i < num_modules; i++) {
    err = init_priv_ops->register_module(modules[i]);
    if (err) {
      log_ops->log_err("INIT", "Failed to register module: %s",
                       modules[i].display_name);
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
    .initialize = init_init,
    .destroy = init_destroy,
};

struct InitOps *get_init_ops(void) { return &init_ops; }

struct InitPrivateOps *get_init_priv_ops(void) { return &_init_priv_ops; }
