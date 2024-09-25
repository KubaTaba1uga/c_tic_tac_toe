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
#include "string.h"
#include <unistd.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "game/game_state_machine/game_state_machine.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct GamePrivateOps {
  int (*user1_logic)(enum InputEvents input_event);
  int (*user2_logic)(enum InputEvents input_event);
  int (*logic)(enum InputEvents input_event);
};

static int game_init(void);
static int game_process_user1(enum InputEvents input_event);
static int game_process_user2(enum InputEvents input_event);
static int game_process(enum InputEvents input_event);

static char module_id[] = "game";
static struct GamePrivateOps game_priv_ops = {.user1_logic = game_process_user1,
                                              .user2_logic = game_process_user2,
                                              .logic = game_process};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_game_reg = {
    .id = module_id,
    .init_func = game_init,
    .destroy_func = NULL,
};
struct InitRegistrationData *init_game_reg_p = &init_game_reg;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameOps game_ops = {};
struct GameOps *get_game_ops(void) { return &game_ops; }

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int game_process_user1(enum InputEvents input_event) {
  return game_sm_ops.step(input_event, User1);
}

int game_process_user2(enum InputEvents input_event) {
  return game_sm_ops.step(input_event, User2);
}

int game_process(enum InputEvents input_event) {
  return game_sm_ops.step(input_event, UserNone);
}

int game_init(void) {
  struct ConfigRegistrationData user1_input, user2_input;
  struct InputOps *input_ops;
  struct ConfigOps *config_ops;
  int err;

  input_ops = get_input_ops();
  config_ops = get_config_ops();

  user1_input.var_name = "user1_input";
  user1_input.default_value = "keyboard1";
  err = config_ops->register_var(user1_input);
  if (err) {
    logging_utils_ops.log_err(module_id,
                              "Unable to register input device for user 1.");
    return err;
  }

  user2_input.var_name = "user2_input";
  user2_input.default_value = "keyboard1";
  err = config_ops->register_var(user2_input);
  if (err) {
    logging_utils_ops.log_err(module_id,
                              "Unable to register input device for user 2.");

    return err;
  }

  // If there is one input device we do not need to distinguish between users
  //  at input device level. We can make this decision on game level.
  if (strcmp(config_ops->get_var("user1_input"),
             config_ops->get_var("user2_input")) == 0) {
    err = input_ops->register_callback(config_ops->get_var("user1_input"),
                                       game_priv_ops.logic);
    if (err) {
      logging_utils_ops.log_err(
          module_id,
          "Unable to register callback for input device for both users.");
      return err;
    }
  } // Otherwise we need to distinguish inputs at device level.
  else {

    err = input_ops->register_callback(config_ops->get_var("user1_input"),
                                       game_priv_ops.user1_logic);
    if (err) {
      logging_utils_ops.log_err(
          module_id,
          "Unable to register callback for input device for user 1.");

      return err;
    }

    err = input_ops->register_callback(config_ops->get_var("user2_input"),
                                       game_priv_ops.user2_logic);
    if (err) {
      logging_utils_ops.log_err(
          module_id,
          "Unable to register callback for input device for user 2.");

      return err;
    }
  }

  err = input_ops->start();

  if (err) {
    logging_utils_ops.log_err(module_id, "Unable to start input devices.");

    return err;
  }

  return 0;
}

INIT_REGISTER_SUBSYSTEM(&init_game_reg, INIT_MODULE_ORDER_GAME);
