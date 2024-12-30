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
#include "utils/logging_utils.h"

/*******************************************************************************
 *    PRIVATE DECLARATIONS & DEFINITIONS
 ******************************************************************************/
static const char *module_id = "game_state_machines_common";
static struct UserMove *get_last_move(struct GameStateMachineState *state);
static struct LoggingUtilsOps *logging_ops;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/

/*******************************************************************************
 *    INIT BOILERCODE
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameStateMachineCommonOps gsm_common_ops = {.get_last_move =
                                                       get_last_move};

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/
struct UserMove *get_last_move(struct GameStateMachineState *state) {
  logging_ops = get_logging_utils_ops();

  if (state->users_moves_offset == 0) {
    logging_ops->log_err(module_id,
                         "No last move to get. This is unexpected behaviour");
    return NULL;
  }

  return &(state->users_moves[state->users_moves_offset - 1]);
}

struct GameStateMachineCommonOps *get_sm_mini_machines_common_ops(void) {
  return &gsm_common_ops;
}
