#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include "static_array_lib.h"

#include "game/game_state_machine/game_state_machine.h"
#include "game/game_user.h"

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

struct GameConfigOps *get_game_config_ops(void);

#endif
