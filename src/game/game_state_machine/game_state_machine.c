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
#include <errno.h>
#include <stddef.h>
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/user_move/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

struct GameStateMachinePrivOps {
  void (*sanitize_last_move)(void);
  int (*is_input_user_valid)(enum Users input_user);
  int (*is_input_event_valid)(enum InputEvents input_event);
};

static int game_sm_init(void);
static void sanitize_last_move(void);
static int validate_input_user(enum Users input_user);
static int validate_input_event(enum InputEvents input_event);
static int game_sm_step(struct GameStateMachineInput data);

static struct GameStateMachineState game_sm;
static char module_id[] = "game_state_machine";
static struct GameStateMachinePrivOps game_sm_priv_ops = {
    .is_input_event_valid = validate_input_event,
    .is_input_user_valid = validate_input_user,
    .sanitize_last_move = sanitize_last_move,
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_game_sm_reg = {
    .id = module_id,
    .init_func = game_sm_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameStateMachineOps game_sm_ops = {.step = game_sm_step};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int game_sm_init(void) {
  game_sm.users_moves_count = 0;
  game_sm.current_state = GameStatePlay;
  game_sm.current_user = User1;

  return 0;
}

// Next step is taken from game_logic_sm, while output is users_moves.
int game_sm_step(struct GameStateMachineInput data) {
  if (game_sm_priv_ops.is_input_event_valid(data.input_event) != 0 ||
      game_sm_priv_ops.is_input_user_valid(data.input_user) != 0)
    return EINVAL;

  logging_utils_ops.log_info(module_id,
                             "Performing step of game state machine.");

  game_sm_priv_ops.sanitize_last_move();

  struct UserMoveCreationData user_move_creation_data = {
      .user = game_sm.current_user,
      .input = data.input_event,
      .count = game_sm.users_moves_count,
      .users_moves = game_sm.users_moves_data};

  struct GameSmNextStateCreationData game_state_creation_data = {
      .current_state = game_sm.current_state,
      .current_user_move = user_move_ops.create_move(user_move_creation_data),
      .count = game_sm.users_moves_count,
      .users_moves = game_sm.users_moves_data};

  game_sm.current_state =
      game_sm_subsystem_ops.next_state(game_state_creation_data);

  game_sm.users_moves_data[game_sm.users_moves_count++] =
      game_state_creation_data.current_user_move;

  return 0;
}

int validate_input_user(enum Users input_user) {
  return game_sm.current_user != input_user;
}

int validate_input_event(enum InputEvents input_event) {
  if (input_event <= INPUT_EVENT_NONE || input_event >= INPUT_EVENT_MAX)
    return 1;
  return 0;
}

void sanitize_last_move(void) {
  if (game_sm.users_moves_count > 0) {
    switch (game_sm.current_state) {
    case GameStatePlay:
      if (game_sm.users_moves_data[game_sm.users_moves_count - 1].type !=
          USER_MOVE_TYPE_SELECT_VALID) {
        game_sm.users_moves_count--;
        break;
      }

    case GameStateWin:
    case GameStateQuitting:
    case GameStateQuit:
      game_sm.users_moves_count--;
      break;
    }
  }
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_reg, init_game_reg_p);
