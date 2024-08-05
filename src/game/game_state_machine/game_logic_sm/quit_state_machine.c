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
#include "game/quit_state_machine.h"
#include "game/game.h"
#include "game/user_move/user_move.h"
#include "init/init.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static int quit_state_machine_init(void);
enum GameStates quit_state_machine_next_state(struct UserMove user_move,
                                              enum GameStates state);

static char module_id[] = "quit_state_machine";
struct GameStateMachineRegistrationData gsm_registration_data = {
    .next_state = quit_state_machine_next_state, .id = module_id};

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
  game_ops.register_state_machine(gsm_registration_data);
  return 0;
}

enum GameStates quit_state_machine_next_state(struct UserMove user_move,
                                              enum GameStates state) {
  switch (state) {
  case (User1):
  case (User2):
    if (user_move.type == USER_MOVE_TYPE_QUIT)
      return Quitting;
  case (Quitting):
    if (user_move.type == USER_MOVE_TYPE_QUIT)
      return Quit;
    else if (user_move.type == USER_MOVE_TYPE_SELECT_VALID ||
             user_move.type == USER_MOVE_TYPE_SELECT_INVALID)
      return Resuming;
    break;
  default:
    return state;
    ;
  }
};

INIT_REGISTER_SUBSYSTEM_CHILD(&init_quit_state_machine_reg, init_game_reg_p);
