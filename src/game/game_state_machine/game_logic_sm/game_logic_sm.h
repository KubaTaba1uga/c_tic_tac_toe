#ifndef GAME_LOGIC_SM_H
#define GAME_LOGIC_SM_H
/*******************************************************************************
 * @file game_logic_sm.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/user_move/user_move.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
struct GameStateCreationData {
  enum GameStates current_state;
  struct UserMove user_move;
  struct UserMove *users_moves;
  size_t count;
};

struct GameLogicSMRegistrationData {
  enum GameStates (*next_state)(struct GameStateCreationData data);
  const char *id;
};

struct GameLogicSMOps {
  enum GameStates (*next_state)(struct GameStateCreationData data);
  void (*register_state_machine)(
      struct GameLogicSMRegistrationData *registration_data);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameLogicSMOps game_logic_sm_ops;

#endif // GAME_LOGIC_SM_H
