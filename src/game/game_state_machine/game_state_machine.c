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
#include <string.h>

// App's internal libs
#include "config/config.h"
#include "game/game.h"
#include "game/game_state_machine/game_logic_sm/game_logic_sm.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/user_move/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct GameStateMachinePrivOps {
  int (*is_input_user_valid)(enum Users input_user);
  int (*is_input_event_valid)(enum InputEvents input_event);
  enum GameStates (*next_state)(struct UserMove user_move);
  int (*process_state)(void);
};

static int game_sm_step(enum InputEvents input_event, enum Users input_user);
static int validate_input_user(enum Users input_user);
static int validate_input_event(enum InputEvents input_event);
static int game_sm_init(void);

static char module_id[] = "game_state_machine";
static struct GameStateMachine game_sm;
static struct GameStateMachinePrivOps game_sm_priv_ops = {
    .is_input_event_valid = validate_input_event,
    .is_input_user_valid = validate_input_user,
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
  game_sm.count = 0;
  game_sm.state = GameStateUser1;
  return 0;
}

int game_sm_step(enum InputEvents input_event, enum Users input_user) {
  if (game_sm_priv_ops.is_input_event_valid(input_event) != 0 ||
      game_sm_priv_ops.is_input_user_valid(input_user) != 0)
    return EINVAL;
  // TO-DO get rid of hilights and invalid moves, aka sanitize users_moves.

  struct UserMoveCreationData user_move_creation_data = {
      .user = game_sm.state,
      .input = input_event,
      .count = game_sm.count,
      .users_moves = game_sm.users_moves};

  struct GameStateCreationData game_state_creation_data = {
      .current_state = game_sm.state,
      .user_move = user_move_ops.create_move(user_move_creation_data),
      .count = game_sm.count,
      .users_moves = game_sm.users_moves};

  game_sm.state = game_logic_sm_ops.next_state(game_state_creation_data);

  game_sm.users_moves[game_sm.count++] = game_state_creation_data.user_move;

  return 0;
}

int validate_input_user(enum Users input_user) {
  switch (input_user) {
  case User1:
    if (game_sm.state != User1)
      return 1;
  case User2:
    if (game_sm.state != User2)
      return 2;
  default:
    return 0;
  }
}

int validate_input_event(enum InputEvents input_event) {
  if (input_event <= INPUT_EVENT_NONE || input_event >= INPUT_EVENT_MAX)
    return 1;
  return 0;
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_reg, init_game_reg_p);
