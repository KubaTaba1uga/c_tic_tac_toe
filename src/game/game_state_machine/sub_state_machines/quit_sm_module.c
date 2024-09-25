/*******************************************************************************
 * @file quit_sm_module.c
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

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/sub_state_machines/common.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/

static int quit_state_machine_init(void);
static int quit_state_machine_next_state(struct GameStateMachineInput input,
                                         struct GameStateMachineState *state);
static char gsm_quit_module_id[] = "quit_sm_module";
static struct GameSmSubsystemRegistrationData gsm_registration_data = {
    .next_state = quit_state_machine_next_state,
    .id = gsm_quit_module_id,
    .priority = 0 // No priority
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_quit_state_machine_reg = {
    .id = gsm_quit_module_id,
    .init_func = quit_state_machine_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int quit_state_machine_next_state(struct GameStateMachineInput input,
                                  struct GameStateMachineState *state) {

  struct UserMove *current_user_move = gsm_common_ops.get_last_move(state);

  switch (state->current_state) {

  case (GameStatePlay):
    if (current_user_move->type == USER_MOVE_TYPE_QUIT) {
      state->current_state = GameStateQuitting;
    }
    break;

  case (GameStateQuitting):
    // If user confirms quitting, just quit.
    if (current_user_move->type == USER_MOVE_TYPE_QUIT) {
      state->current_state = GameStateQuit;
      game_sm_ops.quit();
    }
    // If user cancels quitting, return to play.
    else if (current_user_move->type == USER_MOVE_TYPE_SELECT_VALID ||
             current_user_move->type == USER_MOVE_TYPE_SELECT_INVALID) {
      state->current_state = GameStatePlay;
    }
    break;

  default:;
  }

  return 0;
};

int quit_state_machine_init(void) {
  game_sm_subsystem_ops.register_state_machine(&gsm_registration_data);
  return 0;
}

INIT_REGISTER_SUBSYSTEM_CHILD(&init_quit_state_machine_reg, init_game_reg_p);
