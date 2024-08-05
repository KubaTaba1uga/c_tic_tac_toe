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
#include "game/game_state_machine/game_state_machine.h"
#include "input/input.h"

/*******************************************************************************
 *    PRIVATE API
 ******************************************************************************/

/*******************************************************************************
 *    PUBLIC API
 ******************************************************************************/
enum UserMoveType {
  USER_MOVE_TYPE_MENU,
  USER_MOVE_TYPE_HIGHLIGHT,
  USER_MOVE_TYPE_SELECT_VALID,
  USER_MOVE_TYPE_SELECT_INVALID,
  USER_MOVE_TYPE_QUIT,
};

struct UserMove {
  enum UserMoveType type;
  enum GameStates user;
  int coordinates[2];
};

struct UserMoveCreationData {
  enum GameStates user;
  enum InputEvents input;
  size_t count;
  struct UserMove *users_moves;
};

struct UserMoveOps {
  struct UserMove (*create_move)(struct UserMoveCreationData);
};

/*******************************************************************************
 *    MODULARITY BOILERCODE
 ******************************************************************************/
extern struct UserMoveOps user_move_ops;

#endif // USER_MOVE_H
