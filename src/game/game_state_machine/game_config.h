#ifndef GAME_USER_MOVE_H
#define GAME_USER_MOVE_H

#include "static_array_lib.h"

#include "game/game_state_machine/game_state_machine.h"
#include "game/game_state_machine/game_user.h"

struct GameConfig {
  SARRS_FIELD(users, struct GameUser, MAX_USERS);
  int display_id;
};

struct GameGetUserInput {
  game_user_id_t user_id;
  void *private;
};

struct GameGetUserOutput {
  struct GameUser *user;
};

struct GameConfigOps {
  int (*init)(void);
  int (*get_user)(struct GameGetUserInput *, struct GameGetUserOutput *);
};

struct GameOps *get_game_config_ops(void);

#endif
