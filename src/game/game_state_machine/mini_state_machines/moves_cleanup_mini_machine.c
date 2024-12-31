/*******************************************************************************
 * @file clean_last_move_sm_module.c
 * @brief State Machine for Cleaning Last Move
 *
 * This module handles cleaning the last move in the game state.
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
#include "game/game_state_machine/mini_state_machines/moves_cleanup_mini_machine.h"
#include "game/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct GameSmCleanLastMoveModulePrivateOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};

static char gsm_clean_last_move_module_id[] = "clean_last_move_sm_module";
static struct GameStateMachineCommonOps *gsm_common_ops;
static struct GameSmCleanLastMoveModulePrivateOps *priv_ops;
struct GameSmCleanLastMoveModulePrivateOps *
get_game_sm_clean_last_move_module_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
int clean_last_move_state_machine_init(void) {
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();
  priv_ops = get_game_sm_clean_last_move_module_priv_ops();

  struct MiniGameStateMachine mini_machine = {
      .next_state = priv_ops->next_state,
      .display_name = gsm_clean_last_move_module_id,
      .priority = 1}; // always execute first

  gsm_sub_ops->add_mini_state_machine(mini_machine);

  return 0;
}

int clean_last_move_state_machine_next_state(
    struct GameStateMachineInput input, struct GameStateMachineState *state) {
  struct UserMove *last_move = gsm_common_ops->get_last_move(state);
  int err;

  if (!last_move) {
    return 0;
  }

  if (last_move->type == USER_MOVE_TYPE_SELECT_VALID) {
    return 0;
  }

  err = gsm_common_ops->delete_last_move(state);

  if (err) {
    return err;
  }

  return 0;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
static struct GameSmCleanLastMoveModuleOps game_sm_clean_last_move_ops = {
    .init = clean_last_move_state_machine_init};

static struct GameSmCleanLastMoveModulePrivateOps
    game_sm_clean_last_move_priv_ops = {
        .next_state = clean_last_move_state_machine_next_state,
};

struct GameSmCleanLastMoveModuleOps *
get_game_sm_clean_last_move_module_ops(void) {
  return &game_sm_clean_last_move_ops;
}

struct GameSmCleanLastMoveModulePrivateOps *
get_game_sm_clean_last_move_module_priv_ops(void) {
  return &game_sm_clean_last_move_priv_ops;
}
