#ifndef GAME_STATE_MACHINE_COMMON_H
#define GAME_STATE_MACHINE_COMMON_H
/*******************************************************************************
 * @file game_state_machine_common.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "game/game_state_machine/game_state_machine.h"
#include "game/user_move.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameStateMachineCommonOps {
  struct UserMove *(*get_last_move)(struct GameStateMachineState *state);
  int (*add_move)(struct GameStateMachineState *state,
                  struct UserMove user_move);
  int (*delete_last_move)(struct GameStateMachineState *state);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameStateMachineCommonOps *get_sm_mini_machines_common_ops(void);

#endif //  GAME_STATE_MACHINE_COMMON_H
