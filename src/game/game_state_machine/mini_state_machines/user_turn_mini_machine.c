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
#include <stdio.h>

// App's internal libs
#include "game/game.h"
#include "game/game_config.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/user_turn_mini_machine.h"
#include "game/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct GameSmUserTurnModulePrivateOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};
static char gsm_turn_module_id[] = "user_turn_sm_module";
static struct GameStateMachineCommonOps *gsm_common_ops;
static struct GameConfigOps *game_config_ops;
static struct GameOps *game_ops;
static struct GameSmUserTurnModulePrivateOps *turn_priv_ops;
struct GameSmUserTurnModulePrivateOps *get_game_sm_turn_module_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
static int turn_state_machine_init(void) {
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();

  game_ops = get_game_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();
  game_config_ops = get_game_config_ops();
  turn_priv_ops = get_game_sm_turn_module_priv_ops();

  struct MiniGameStateMachine turn_mini_machine = {
      .next_state = turn_priv_ops->next_state,
      .display_name = gsm_turn_module_id,
      .priority = 1}; // always first

  gsm_sub_ops->add_mini_state_machine(turn_mini_machine);

  return 0;
}

static int turn_state_machine_next_state(struct GameStateMachineInput input,
                                         struct GameStateMachineState *state) {
  struct UserMove *current_user_move = gsm_common_ops->get_last_move(state);
  int users_amount;
  int err;

  if (!current_user_move) {
    return 0;
  }

  err = game_config_ops->get_users_amount(&users_amount);
  if (err) {
    return err;
  }

  if (current_user_move->type == USER_MOVE_TYPE_SELECT_VALID)
    state->current_user = (state->current_user + 1) % users_amount;

  return 0;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/

static struct GameSmUserTurnModulePrivateOps private_ops = {
    .next_state = turn_state_machine_next_state,
};

static struct GameSmUserTurnModuleOps game_sm_turn_ops = {
    .init = turn_state_machine_init};

struct GameSmUserTurnModuleOps *get_game_sm_user_turn_module_ops(void) {
  return &game_sm_turn_ops;
}

struct GameSmUserTurnModulePrivateOps *get_game_sm_turn_module_priv_ops(void) {
  return &private_ops;
};
