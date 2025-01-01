#include <asm-generic/errno-base.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display/display.h"
#include "static_array_lib.h"

#include "config/config.h"
#include "game/game_config.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_user.h"
#include "game/user_move.h"
#include "input/input.h"
#include "input/keyboard/keyboard_keys_mapping_1.h"
#include "utils/logging_utils.h"

typedef struct GameConfig GameConfig;

struct GameConfig {
  SARRS_FIELD(users, struct GameUser, MAX_USERS);
  int display_id;
};

SARRS_DECL(GameConfig, users, struct GameUser, MAX_USERS);

static struct InputOps *input_ops;
static struct ConfigOps *config_ops;
static struct LoggingUtilsOps *log_ops;
static GameConfig game_config;

static int game_config_init(void) {
  struct ConfigVariable config_var = {.var_name = "users_amount",
                                      .default_value = "2"};
  struct GameStateMachineOps *gsm_ops = get_game_state_machine_ops();
  struct DisplayOps *display_ops = get_display_ops();
  struct InputGetDeviceExtendedOutput get_device;
  struct ConfigAddVarOutput add_var;
  struct ConfigGetVarOutput get_var;
  struct GameUser user;
  int users_amount;
  int display_id;
  size_t i;
  int err;

  config_ops = get_config_ops();
  log_ops = get_logging_utils_ops();
  input_ops = get_input_ops();

  GameConfig_users_init(&game_config);
  game_config.display_id = 0;

  // Add users_amount config variable
  err = config_ops->add_var((struct ConfigAddVarInput){.var = &config_var},
                            &add_var);
  if (err) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME,
                     "Unable to add users_amount config variable: %s",
                     strerror(err));
    return err;
  }

  err = config_ops->get_var(
      (struct ConfigGetVarInput){.var_id = add_var.var_id,
                                 .mode = CONFIG_GET_VAR_BY_ID},
      &get_var);
  if (err) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME,
                     "Unable to get users_amount config variable: %s",
                     strerror(err));
    return err;
  }

  users_amount = atoi(get_var.value);
  log_ops->log_info(GAME_CONFIG_FILE_NAME, "Users amount: %i", users_amount);

  if (users_amount > MAX_USERS) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME,
                     "Invalid users_amount value, maximum users amount is %d",
                     MAX_USERS);
    return err;
  }

  for (i = 0; i < users_amount; i++) {
    // Add user input config variable
    snprintf(config_var.var_name, CONFIG_VARIABLE_MAX, "user%zu_input", i + 1);
    strcpy(config_var.default_value, KEYBOARD_KEYS_MAPPING_1_DISP_NAME);

    err = config_ops->add_var((struct ConfigAddVarInput){.var = &config_var},
                              &add_var);
    if (err) {
      log_ops->log_err(GAME_CONFIG_FILE_NAME,
                       "Unable to add %s config variable: %s",
                       config_var.var_name, strerror(err));
      return err;
    }

    err = config_ops->get_var(
        (struct ConfigGetVarInput){.var_id = add_var.var_id,
                                   .mode = CONFIG_GET_VAR_BY_ID},
        &get_var);
    if (err) {
      log_ops->log_err(GAME_CONFIG_FILE_NAME,
                       "Unable to get %s config variable: %s",
                       config_var.var_name, strerror(err));
      return err;
    }

    // Get user device id
    err = input_ops->get_device_extended(
        &(struct InputGetDeviceExtendedInput){.device_name = get_var.value,
                                              .mode = INPUT_GET_DEVICE_BY_NAME},
        &get_device);
    if (err) {
      log_ops->log_err(GAME_CONFIG_FILE_NAME,
                       "Unable to get %s input device: %s", get_var.value,
                       strerror(err));
      return err;
    }

    err = input_ops->set_callback(
        (struct InputSetCallbackInput){.callback = gsm_ops->step,
                                       .device_id = get_device.device_id},
        &(struct InputSetCallbackOutput){});
    if (err) {
      log_ops->log_err(GAME_CONFIG_FILE_NAME,
                       "Unable to set callback for %s input device: %s",
                       get_var.value, strerror(err));
      return err;
    }

    user.device_id = get_device.device_id;
    snprintf(user.display_name, GAME_USER_DISP_NAME_MAX, "player_%zu", i);

    // Add user to game config
    err = GameConfig_users_append(&game_config, user);
    if (err) {
      log_ops->log_err(GAME_CONFIG_FILE_NAME, "Unable to add %s user: %s",
                       user.display_name, strerror(err));
      return err;
    }
  }

  err = config_ops->init_var(&config_var, "display", DISPLAY_CLI_NAME);
  if (err) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME,
                     "Unable to init %s config variable: %s", "display",
                     strerror(err));
    return err;
  }

  err = config_ops->add_var((struct ConfigAddVarInput){.var = &config_var},
                            &add_var);
  if (err) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME,
                     "Unable to add %s config variable: %s",
                     config_var.var_name, strerror(err));
    return err;
  }

  err = config_ops->get_var(
      (struct ConfigGetVarInput){.var_id = add_var.var_id,
                                 .mode = CONFIG_GET_VAR_BY_ID},
      &get_var);
  if (err) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME,
                     "Unable to get %s config variable: %s",
                     config_var.var_name, strerror(err));
    return err;
  }

  err = display_ops->get_display_id(get_var.value, &display_id);
  if (err) {
    log_ops->log_err(GAME_CONFIG_FILE_NAME, "Unable to get %s display: %s",
                     get_var.value, strerror(err));
    return err;
  }

  game_config.display_id = display_id;

  return 0;
}

static int game_config_get_user(struct GameGetUserInput *input,
                                struct GameGetUserOutput *output) {
  if (!input || !output)
    return EINVAL;

  return GameConfig_users_get(&game_config, input->user_id, &output->user);
};

static int game_config_get_display_id(int *placeholder) {
  if (!placeholder) {
    return EINVAL;
  }

  *placeholder = game_config.display_id;

  return 0;
};

static int game_config_get_users_amount(int *placeholder) {
  if (!placeholder) {
    return EINVAL;
  }

  *placeholder = GameConfig_users_length(&game_config);

  return 0;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct GameConfigOps game_config_pub_ops = {
    .init = game_config_init,
    .get_user = game_config_get_user,
    .get_display_id = game_config_get_display_id,
    .get_users_amount = game_config_get_users_amount,
};

struct GameConfigOps *get_game_config_ops(void) { return &game_config_pub_ops; }
