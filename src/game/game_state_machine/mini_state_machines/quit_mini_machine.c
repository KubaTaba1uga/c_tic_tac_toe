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
#include "game/game_state_machine/sub_state_machines/quit_sm_module.h"
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
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmQuitModulePrivateOps {
  int (*init)(void);
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};

struct GameSmQuitModulePrivateOps private_ops = {
    .init = quit_state_machine_init,
    .next_state = quit_state_machine_next_state,

};

static struct GameSmQuitModuleOps game_sm_quit_ops = {.private_ops =
                                                          &private_ops};

struct GameSmQuitModuleOps *get_game_sm_quit_module_ops(void) {
  return &game_sm_quit_ops;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_quit_state_machine_reg = {
    .id = gsm_quit_module_id,
    .init = quit_state_machine_init,
    .destroy = NULL,
};

/*******************************************************************************
 *    API
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
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();

  gsm_sub_ops->register_state_machine(&gsm_registration_data);

  return 0;
}
