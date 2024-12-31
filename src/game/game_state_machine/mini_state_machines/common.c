/*******************************************************************************
 * @file common.c
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
#include "game/game_state_machine/mini_state_machines/common.h"
#include "game/game_state_machine/game_state_machine.h"
#include "utils/logging_utils.h"
#include <asm-generic/errno-base.h>

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static struct LoggingUtilsOps *logging_ops;

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
static struct UserMove *get_last_move(struct GameStateMachineState *state) {
  logging_ops = get_logging_utils_ops();

  if (state->users_moves_offset == 0) {
    return NULL;
  }

  return &(state->users_moves[state->users_moves_offset - 1]);
}

static int add_move(struct GameStateMachineState *state,
                    struct UserMove user_move) {
  if (!state || state->users_moves_offset + 1 > MAX_USERS_MOVES)
    return EINVAL;

  state->users_moves[state->users_moves_offset++] = user_move;

  return 0;
};

static int delete_last_move(struct GameStateMachineState *state) {

  if (!state)
    return EINVAL;

  if (state->users_moves_offset == 0)
    return 0;

  state->users_moves_offset--;

  return 0;
};

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
static struct GameStateMachineCommonOps gsm_common_ops = {
    .get_last_move = get_last_move,
    .add_move = add_move,
    .delete_last_move = delete_last_move,
};

struct GameStateMachineCommonOps *get_sm_mini_machines_common_ops(void) {
  return &gsm_common_ops;
}
