#ifndef GAME_SM_SUBSYSTEM_H
#define GAME_SM_SUBSYSTEM_H
/*******************************************************************************
 * @file game_sm_subsystem.h
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
struct GameSmNextStateCreationData {
  size_t count;
  struct UserMove *users_moves;
  enum GameStates current_state;
  struct UserMove current_user_move;
};

struct GameSmSubsystemRegistrationData {
  const char *id;
  enum GameStates (*callback)(struct GameSmNextStateCreationData data);
};

struct GameSmSubsystemOps {
  void (*register_state_machine)(
      struct GameSmSubsystemRegistrationData *registration_data);
  enum GameStates (*next_state)(struct GameSmNextStateCreationData data);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct GameSmSubsystemOps game_sm_subsystem_ops;

#endif // GAME_SM_SUBSYSTEM_H
