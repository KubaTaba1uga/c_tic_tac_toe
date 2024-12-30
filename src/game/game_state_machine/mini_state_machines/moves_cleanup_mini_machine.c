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
#include "game/game_state_machine/sub_state_machines/common.h"
#include "game/game_state_machine/sub_state_machines/moves_cleanup_sm_module.h"
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static int clean_last_move_state_machine_init(void);
static int
clean_last_move_state_machine_next_state(struct GameStateMachineInput input,
                                         struct GameStateMachineState *state);
static char gsm_clean_last_move_module_id[] = "clean_last_move_sm_module";
static struct GameSmSubsystemRegistrationData gsm_registration_data = {
    .next_state = clean_last_move_state_machine_next_state,
    .id = gsm_clean_last_move_module_id,
    .priority = 1};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmCleanLastMoveModulePrivateOps {
  int (*init)(void);
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};

static struct GameSmCleanLastMoveModulePrivateOps private_ops = {
    .init = clean_last_move_state_machine_init,
    .next_state = clean_last_move_state_machine_next_state,
};

static struct GameSmCleanLastMoveModuleOps game_sm_clean_last_move_ops = {
    .private_ops = &private_ops};

struct GameSmCleanLastMoveModuleOps *
get_game_sm_clean_last_move_module_ops(void) {
  return &game_sm_clean_last_move_ops;
}

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/
struct InitRegistrationData init_clean_last_move_state_machine_reg = {
    .id = gsm_clean_last_move_module_id,
    .init = clean_last_move_state_machine_init,
    .destroy = NULL,
};

/*******************************************************************************
 *    API
 ******************************************************************************/
int clean_last_move_state_machine_next_state(
    struct GameStateMachineInput input, struct GameStateMachineState *state) {
  struct UserMove *last_move = gsm_common_ops.get_last_move(state);

  if (!last_move) {
    return 0; // No move to clean
  }

  if (last_move->type == USER_MOVE_TYPE_SELECT_VALID) {
    return 0;
  }

  // Logic to clean the last move
  if (state->users_moves_count > 0) {
    state->users_moves_count--;
  }

  return 0;
};

int clean_last_move_state_machine_init(void) {
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();

  gsm_sub_ops->register_state_machine(&gsm_registration_data);

  return 0;
}
