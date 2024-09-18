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
#include "game/game_state_machine/game_states.h"
#include "game/game_state_machine/user_move/user_move.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define MAX_USERS_MOVES 100

struct GameStateMachineInput {
  enum InputEvents input_event;
  enum Users input_user;
};

struct GameStateMachineOutput {
  enum GameStates next_state;
  enum Users next_user;
};

struct GameStateMachineState {
  struct UserMove users_moves_data[MAX_USERS_MOVES];
  enum GameStates current_state;
  size_t users_moves_count;
  enum Users current_user;
};

struct GameStateMachineOps {
  int (*step)(struct GameStateMachineInput);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameStateMachineOps game_sm_ops;

#endif // GAME_STATE_MACHINE_H
