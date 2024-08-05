/*******************************************************************************
 * @file game_state_machine.c
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
// C standard library
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "game/user_move/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
#define MAX_USERS_MOVES 100

struct GameStateMachine {
  struct UserMove users_moves[MAX_USERS_MOVES];
  enum Users state;
  size_t count;
};

struct GameStateMachine game_state_machine;
static char module_id[] = "game";

static int game_state_machine_init(void);
static int game_state_machine_step(enum InputEvents input_event);

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_game_state_machine_reg = {
    .id = module_id,
    .init_func = game_state_machine_init,
    .destroy_func = NULL,
};
struct InitRegistrationData *init_game_reg_p = &init_game_state_machine_reg;

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
int game_state_machine_step_user1(enum InputEvents input_event) {
  if (game_state_machine.state != User1)
    return 0;

  return game_state_machine_step(input_event);
}

int game_state_machine_step_user2(enum InputEvents input_event) {
  if (game_state_machine.state != User2)
    return 0;

  return game_state_machine_step(input_event);
}

int game_state_machine_step(enum InputEvents input_event) {
  if (input_event <= INPUT_EVENT_NONE || input_event >= INPUT_EVENT_MAX)
    return 0;

  struct UserMove user_move;
  struct UserMoveCreationData user_move_creation_data = {
      .user = game_state_machine.state,
      .input = input_event,
      .count = game_state_machine.count,
      .users_moves = game_state_machine.users_moves};

  user_move = user_move_ops.create_move(user_move_creation_data);

  (void)user_move;

  return 0;
}

int game_state_machine_init(void) {
  struct ConfigRegistrationData user1_input = {.var_name = "user1_input",
                                               .default_value = "keyboard1"};
  struct ConfigRegistrationData user2_input = {.var_name = "user2_input",
                                               .default_value = "keyboard1"};
  int err;

  // State 0
  game_state_machine.state = User1;
  game_state_machine.count = 0;

  err = config_ops.register_var(user1_input);
  if (err) {
    logging_utils_ops.log_err(module_id,
                              "Unable to register input device for user 1.");

    return err;
  }

  err = config_ops.register_var(user2_input);
  if (err) {
    logging_utils_ops.log_err(module_id,
                              "Unable to register input device for user 2.");

    return err;
  }

  // If there is one input device we do not need to distinguish between users
  //  at input device level. We can make this decision on game level.
  if (strcmp(config_ops.get_var("user1_input"),
             config_ops.get_var("user2_input")) == 0) {
    err = input_ops.register_callback(config_ops.get_var("user1_input"),
                                      game_state_machine_step);
    if (err) {
      logging_utils_ops.log_err(
          module_id,
          "Unable to register callback for input device for both users.");
      return err;
    }

    return 0;
  }

  // Otherwise we need to distinguish inputs at device level.
  err = input_ops.register_callback(config_ops.get_var("user1_input"),
                                    game_state_machine_step_user1);
  if (err) {
    logging_utils_ops.log_err(
        module_id, "Unable to register callback for input device for user 1.");

    return err;
  }

  err = input_ops.register_callback(config_ops.get_var("user2_input"),
                                    game_state_machine_step_user2);
  if (err) {
    logging_utils_ops.log_err(
        module_id, "Unable to register callback for input device for user 2.");

    return err;
  }

  return 0;
}

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
INIT_REGISTER_SUBSYSTEM(&init_game_state_machine_reg, INIT_MODULE_ORDER_GAME);
