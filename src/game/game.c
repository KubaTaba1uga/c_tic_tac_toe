/*******************************************************************************
 * @file game.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// App's internal libs
#include "config/config.h"
#include "display/display.h"
#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/array_utils.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define GAME_USERS_AMOUNT_MAX 10

struct GamePrivateOps {
  int (*logic)(enum InputEvents input_event);
  int (*logic_and_display)(enum InputEvents input_event);
};

struct GameSubsystem {
  array_t env_vars;
};

static struct GameSubsystem game_subsystem;
static char module_id[] = "game";

static struct GamePrivateOps *game_priv_ops;
static struct GamePrivateOps *get_game_private_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
int game_init_system(void) {
  struct ConfigRegistrationData *users_amount_env, *user_n_input;
  struct LoggingUtilsOps *logging_ops;
  struct ArrayUtilsOps *array_ops;
  const size_t buffer_size = 100;
  char buffer_str[buffer_size];
  struct ConfigOps *config_ops;
  /* struct InputOps *input_ops; */
  int users_amount;
  size_t i;
  int err;

  game_priv_ops = get_game_private_ops();
  logging_ops = get_logging_utils_ops();
  array_ops = get_array_utils_ops();
  config_ops = get_config_ops();
  /* input_ops = get_input_ops(); */

  err = array_ops->init(&game_subsystem.env_vars, GAME_USERS_AMOUNT_MAX);
  if (err) {
    logging_ops->log_err(module_id, "Unable to init array for env variables.");
    return err;
  }

  users_amount_env = malloc(sizeof(struct ConfigRegistrationData));
  if (!users_amount_env) {
    logging_ops->log_err(module_id,
                         "Unable to allocate memory for 'users amount' env");
    return ENOMEM;
  }

  err = config_ops->init(users_amount_env, "users_amount", "2");
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to initialize env for 'users amount': %s",
                         strerror(err));
    return err;
  }

  err = config_ops->register_system_var(users_amount_env);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to register env for 'users amount': %s",
                         strerror(err));
    return err;
  }

  users_amount =
      atoi(config_ops->get_system_var((char *)users_amount_env->var_name));

  if (0 >= users_amount) {
    logging_ops->log_err(
        module_id, "Invalid 'users amount' value: %s",
        config_ops->get_system_var((char *)users_amount_env->var_name));
    return err;
  }

  err = array_ops->append(game_subsystem.env_vars, users_amount_env);
  if (err) {
    logging_ops->log_err(module_id,
                         "Unable to add 'users amount' env to array: %s",
                         strerror(err));
    return err;
  }

  for (i = 0; i < users_amount; i++) {
    user_n_input = malloc(sizeof(struct ConfigRegistrationData));
    if (!user_n_input) {
      logging_ops->log_err(module_id, "Unable to allocate memory for user: %i",
                           i);
      return ENOMEM;
    }

    sprintf(buffer_str, "user_%lu_input", i);

    err = config_ops->init(user_n_input, buffer_str, "keyboard1");
    if (err) {
      logging_ops->log_err(module_id, "Unable to init env for '%s': %s",
                           buffer_str, strerror(err));
      return err;
    }

    err = config_ops->register_system_var(user_n_input);
    if (err) {
      logging_ops->log_err(module_id, "Unable to register env for '%s': %s",
                           buffer_str, strerror(err));
      return err;
    }

    err = array_ops->append(game_subsystem.env_vars, user_n_input);
    if (err) {
      logging_ops->log_err(module_id,
                           "Unable to add 'users amount' env to array: %s",
                           strerror(err));
      return err;
    }
  };

  /* err =
   * input_ops->register_callback(config_ops->get_system_var("user1_input"), */
  /*                                    game_priv_ops.logic); */
  /* if (err) { */
  /*   logging_ops->log_err( */
  /*       module_id, */
  /*       "Unable to register callback for input device for both users."); */
  /*   return err; */
  /* } */

  /* err = input_ops->start(); */

  /* if (err) { */
  /*   logging_ops->log_err(module_id, "Unable to start input devices."); */

  /*   return err; */
  /* } */

  return 0;
}

int game_process(enum InputEvents input_event) {
  return game_priv_ops->logic(input_event);
}

int game_process_and_display(enum InputEvents input_event) {
  /* struct GameStateMachineState *game_state_machine; */
  /* struct LoggingUtilsOps *logging_ops; */
  /* struct DataToDisplay display_data; */
  /* struct DisplayOps *display_ops; */

  /* int err; */

  /* logging_ops = get_logging_utils_ops(); */
  /* display_ops = get_display_ops(); */

  /* err = game_sm_ops.step(input_event); */
  /* if (err) { */
  /*   logging_ops->log_err(module_id, "Unable to perform step for user %i.", */
  /*                        input_user); */

  /*   return err; */
  /* } */

  /* game_state_machine = game_sm_ops.get_state_machine(); */
  /* if (!game_state_machine) { */
  /*   logging_ops->log_err(module_id, "No state machine."); */

  /*   return EINVAL; */
  /* } */

  /* display_data.user = game_state_machine->current_user; */
  /* display_data.state = game_state_machine->current_state; */
  /* display_data.moves_counter = game_state_machine->users_moves_count; */
  /* memcpy(display_data.moves, game_state_machine->users_moves_data, */
  /*        sizeof(game_state_machine->users_moves_data)); */

  /* err = display_ops->display(&display_data); */
  /* if (err) { */
  /*   logging_ops->log_err(module_id, "Unable to display game for user %i.", */
  /*                        input_user); */
  /*   return err; */
  /* } */

  return 0;
}

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct GamePrivateOps game_priv_ops_ = {
    .logic = game_process,
    .logic_and_display = game_process_and_display,
};

static struct GamePrivateOps *get_game_private_ops(void) {
  return &game_priv_ops_;
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_game_reg = {
    .id = module_id,
    .init = game_init_system,
    .destroy = NULL // game_destroy_system,
};
