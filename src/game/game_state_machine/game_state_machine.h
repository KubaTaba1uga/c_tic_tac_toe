#ifndef GAME_STATE_MACHINE_H
#define GAME_STATE_MACHINE_H
/*******************************************************************************
 * @file game_state_machine.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "static_array_lib.h"

#include "game/game_state_machine/game_states.h"
#include "game/game_user.h"
#include "game/user_move.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define MAX_USERS_MOVES 1000
#define MAX_USERS 10

struct GameStateMachineInput {
  enum InputEvents input_event;
  input_device_id_t device_id;
};

struct GameStateMachineState {
  SARRS_FIELD(users_moves, struct UserMove, MAX_USERS_MOVES);
  game_user_id_t current_user;
  enum GameStates current_state;
};

struct GameStateMachineOps {};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameStateMachineOps *get_game_state_machine_ops(void);

#endif // GAME_STATE_MACHINE_H
