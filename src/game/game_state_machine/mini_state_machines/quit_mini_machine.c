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
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/quit_mini_machine.h"
#include "game/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct GameSmQuitModulePrivateOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};
static char gsm_quit_module_id[] = "quit_sm_module";
static struct GameSmQuitModulePrivateOps *quit_priv_ops;
struct GameSmQuitModulePrivateOps *get_game_sm_quit_module_priv_ops(void);
static struct GameStateMachineCommonOps *gsm_common_ops;
static struct GameOps *game_ops;

/*******************************************************************************
 *    API
 ******************************************************************************/
int quit_state_machine_init(void) {
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();

  game_ops = get_game_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();
  quit_priv_ops = get_game_sm_quit_module_priv_ops();

  struct MiniGameStateMachine quit_mini_machine = {
      .next_state = quit_priv_ops->next_state,
      .display_name = gsm_quit_module_id,
      .priority = 0};

  gsm_sub_ops->add_mini_state_machine(quit_mini_machine);

  return 0;
}

int quit_state_machine_next_state(struct GameStateMachineInput input,
                                  struct GameStateMachineState *state) {
  struct UserMove *current_user_move = gsm_common_ops->get_last_move(state);

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
      game_ops->stop();
      return 0;
    }
    // If user cancels quitting, return to play.
    state->current_state = GameStatePlay;
    break;

  default:;
  }

  return 0;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/

static struct GameSmQuitModulePrivateOps private_ops = {
    .next_state = quit_state_machine_next_state,
};

static struct GameSmQuitModuleOps game_sm_quit_ops = {
    .init = quit_state_machine_init};

struct GameSmQuitModuleOps *get_game_sm_quit_module_ops(void) {
  return &game_sm_quit_ops;
}

struct GameSmQuitModulePrivateOps *get_game_sm_quit_module_priv_ops(void) {
  return &private_ops;
};
