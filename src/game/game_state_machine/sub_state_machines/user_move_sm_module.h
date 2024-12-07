#ifndef USER_MOVE_H
#define USER_MOVE_H
/*******************************************************************************
 * @file user_move.h
 * @brief TO-DO
 *
 * TO-DO
 *
 ******************************************************************************/

/*******************************************************************************
 *    IMPORTS
 ******************************************************************************/
#include <stddef.h>

#include "game/game.h"
#include "game/game_state_machine/game_states.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
enum UserMoveType {
  USER_MOVE_TYPE_HIGHLIGHT,
  USER_MOVE_TYPE_SELECT_VALID,
  USER_MOVE_TYPE_SELECT_INVALID,
  USER_MOVE_TYPE_QUIT,
};

struct UserMove {
  enum UserMoveType type;
  enum Users user;
  int coordinates[2];
};

extern struct InitRegistrationData init_user_move_reg;

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
struct GameSmUserMoveModuleOps {
  void *private_ops;
};

struct GameSmUserMoveModuleOps *get_game_sm_user_move_module_ops(void);

#endif // USER_MOVE_H
