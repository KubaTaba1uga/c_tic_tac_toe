/*******************************************************************************
 * @file display_sm_module.c
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
#include "display/display.h"
#include "game/game.h"
#include "game/game_config.h"
#include "game/game_state_machine/game_sm_subsystem.h"
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/mini_state_machines/display_mini_machine.h"
#include "game/user_move.h"
#include "init/init.h"
#include "input/input.h"
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
struct GameSmDisplayModulePrivateOps {
  int (*next_state)(struct GameStateMachineInput input,
                    struct GameStateMachineState *state);
};

static struct DisplayOps *display_ops;
static struct GameConfigOps *game_config_ops;
static struct GameStateMachineCommonOps *gsm_common_ops;
static char gsm_display_module_id[] = "display_sm_module";
static struct GameSmDisplayModulePrivateOps *display_priv_ops;
struct GameSmDisplayModulePrivateOps *get_game_sm_display_module_priv_ops(void);

/*******************************************************************************
 *    API
 ******************************************************************************/
int display_state_machine_init(void) {
  struct GameSmSubsystemOps *gsm_sub_ops = get_game_sm_subsystem_ops();

  game_config_ops = get_game_config_ops();
  gsm_common_ops = get_sm_mini_machines_common_ops();
  display_priv_ops = get_game_sm_display_module_priv_ops();

  struct MiniGameStateMachine display_mini_machine = {
      .next_state = display_priv_ops->next_state,
      .display_name = gsm_display_module_id,
      .priority = -1}; // Always last

  gsm_sub_ops->add_mini_state_machine(display_mini_machine);

  return 0;
}

int display_state_machine_next_state(struct GameStateMachineInput input,
                                     struct GameStateMachineState *state) {
  int display_id;
  int err;

  err = game_config_ops->get_display_id(&display_id);
  if (err) {
    return err;
  }

  struct DisplayData display_data = {.game_state = state->current_state,
                                     .moves = state->users_moves,
                                     .moves_length = state->users_moves_offset,
                                     .user_id = state->current_user,
                                     .display_id = display_id};

  err = display_ops->display(&display_data);
  if (err) {
    return err;
  }

  return 0;
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/

static struct GameSmDisplayModulePrivateOps private_ops = {
    .next_state = display_state_machine_next_state,
};

static struct GameSmDisplayModuleOps game_sm_display_ops = {
    .init = display_state_machine_init};

struct GameSmDisplayModuleOps *get_game_sm_display_module_ops(void) {
  return &game_sm_display_ops;
}

struct GameSmDisplayModulePrivateOps *
get_game_sm_display_module_priv_ops(void) {
  return &private_ops;
};
