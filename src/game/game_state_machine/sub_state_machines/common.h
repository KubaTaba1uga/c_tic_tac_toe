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
#include "game/game_state_machine/sub_state_machines/user_move_sm_module.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameStateMachineCommonOps {
  struct UserMove *(*get_last_move)(struct GameStateMachineState *state);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameStateMachineCommonOps gsm_common_ops;

#endif // COMMON_H
