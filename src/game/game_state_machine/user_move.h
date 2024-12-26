#ifndef GAME_USER_MOVE_H
#define GAME_USER_MOVE_H

#include "game/game_state_machine/game_user.h"

struct UserMoveCoordinates {
  int x;
  int y;
};

enum UserMoveType {
  USER_MOVE_TYPE_HIGHLIGHT,
  USER_MOVE_TYPE_SELECT_VALID,
  USER_MOVE_TYPE_SELECT_INVALID,
  USER_MOVE_TYPE_QUIT,
};

struct UserMove {
  enum UserMoveType type;
  game_user_id_t user_id;
  struct UserMoveCoordinates coordinates;
};

#endif
