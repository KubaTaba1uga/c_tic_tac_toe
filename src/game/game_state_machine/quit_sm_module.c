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

// App's internal libs
#include "game/game.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/user_move/user_move.h"
#include "init/init.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct QuitSmData {
  enum GameStates last_user;
};

static int quit_state_machine_init(void);
static struct GameStateMachineState
quit_state_machine_next_state(struct GameStateMachineInput input,
                              struct GameStateMachineState state);
/* static void quit_state_machine_quit(void); */

struct QuitSmData quit_sm_data;
static char module_id[] = "quit_state_machine";
struct GameSmSubsystemRegistrationData gsm_registration_data = {
    .next_state = quit_state_machine_next_state,
    .id = module_id,
    .priority = 0 // No priority
};

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
static struct InitRegistrationData init_quit_state_machine_reg = {
    .id = module_id,
    .init_func = quit_state_machine_init,
    .destroy_func = NULL,
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
int quit_state_machine_init(void) {
  game_sm_subsystem_ops.register_state_machine(&gsm_registration_data);
  return 0;
}

struct GameStateMachineState
quit_state_machine_next_state(struct GameStateMachineInput input,
                              struct GameStateMachineState state) {
  switch (state.current_state) {
    /* case (GameStatePlay): */

    /*   if (state.current_user_move.type == USER_MOVE_TYPE_QUIT) { */
    /*     quit_sm_data.last_user = state.current_state; */
    /*     state.current_state = GameStateQuitting; */
    /*   } */
    /*   break; */

    /* case (GameStateQuitting): */
    /*   // If user confirms quitting, just quit. */
    /*   if (state.current_user_move.type == USER_MOVE_TYPE_QUIT) { */
    /*     quit_state_machine_quit(); */
    /*     state.current_state = GameStateQuit; */
    /*   } */

    /*   // If user cancels quitting, return to last user turn. */
    /*   else if (state.current_user_move.type == USER_MOVE_TYPE_SELECT_VALID ||
     */
    /*            state.current_user_move.type == USER_MOVE_TYPE_SELECT_INVALID)
     * { */
    /*     state.current_state = GameStatePlay; */
    /*   } */
    /*   break; */

  default:;
  }

  return state;
};

/* void quit_state_machine_quit(void) { input_ops.destroy(); } */

INIT_REGISTER_SUBSYSTEM_CHILD(&init_quit_state_machine_reg, init_game_reg_p);
