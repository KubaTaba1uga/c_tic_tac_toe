/*******************************************************************************
 * @file quit_state_machine.c
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
#include "game/game_state_machine/game_logic_sm/game_logic_sm.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/user_move/user_move.h"
#include "init/init.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct QuitSM {
  enum GameStates last_user;
};

static int quit_state_machine_init(void);
enum GameStates
quit_state_machine_next_state(struct GameStateCreationData data);

static char module_id[] = "quit_state_machine";
struct GameLogicSMRegistrationData gsm_registration_data = {
    .next_state = quit_state_machine_next_state, .id = module_id};
struct QuitSM quit_sm;

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
  game_logic_sm_ops.register_state_machine(&gsm_registration_data);
  return 0;
}

enum GameStates
quit_state_machine_next_state(struct GameStateCreationData data) {
  switch (data.current_state) {
  case (GameStateUser1):
  case (GameStateUser2):
    if (data.user_move.type == USER_MOVE_TYPE_QUIT) {
      quit_sm.last_user = data.current_state;
      return GameStateQuitting;
    }

  case (GameStateQuitting):
    if (data.user_move.type == USER_MOVE_TYPE_QUIT) {
      return GameStateQuit;
    } else if (data.user_move.type == USER_MOVE_TYPE_SELECT_VALID ||
               data.user_move.type == USER_MOVE_TYPE_SELECT_INVALID) {
      return quit_sm.last_user;
    }

  default:
    return data.current_state;
    ;
  }
};

INIT_REGISTER_SUBSYSTEM_CHILD(&init_quit_state_machine_reg, init_game_reg_p);
