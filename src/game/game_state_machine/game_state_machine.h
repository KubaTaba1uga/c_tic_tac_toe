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
#include "game/game.h"
#include "game/user_move/user_move.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
#define MAX_USERS_MOVES 100

enum GameStates {
  GameStateUser1 = User1,
  GameStateUser2 = User2,
  GameStateQuitting,
  GameStateQuit,
  GameStateWin,
};

struct GameStateMachine {
  struct UserMove users_moves[MAX_USERS_MOVES];
  enum GameStates state;
  size_t count;
};

struct GameStateMachineRegistrationData {
  enum GameStates (*next_state)(struct GameStateMachine);
  const char *id;
};

struct GameStateMachineOps {
  int (*step)(enum InputEvents input_event, enum Users input_user);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameStateMachineOps game_sm_ops;

#endif // GAME_STATE_MACHINE_H
