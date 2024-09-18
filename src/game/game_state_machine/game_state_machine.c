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
  int (*is_input_user_valid)(enum Users input_user);
  int (*is_input_event_valid)(enum InputEvents input_event);
};

static int game_sm_init(void);
static int validate_input_user(enum Users input_user);
static int validate_input_event(enum InputEvents input_event);
static int game_sm_step(enum InputEvents input_event, enum Users input_user);

static struct GameStateMachineState game_sm;
static char module_id[] = "game_state_machine";
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
  game_sm.users_moves_count = 0;
  game_sm.current_state = GameStatePlay;
  game_sm.current_user = User1;

  return 0;
}

int game_sm_step(enum InputEvents input_event, enum Users input_user) {
  struct GameStateMachineInput data;

  if (game_sm_priv_ops.is_input_event_valid(input_event) != 0 ||
      game_sm_priv_ops.is_input_user_valid(input_user) != 0)
    return EINVAL;

  logging_utils_ops.log_info(module_id,
                             "Performing step of game state machine");

  data.input_event = input_event;
  data.input_user = input_user;

  game_sm = game_sm_subsystem_ops.next_state(data, game_sm);

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

INIT_REGISTER_SUBSYSTEM_CHILD(&init_game_sm_reg, init_game_reg_p);
