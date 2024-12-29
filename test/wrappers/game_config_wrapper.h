#include "game/game_config.h"
#include "game/user_move.h"
#include "static_array_lib.h"

typedef struct GameConfig GameConfig;

struct GameConfig {
  SARRS_FIELD(users, struct GameUser, MAX_USERS);
  int display_id;
};

SARRS_DECL(GameConfig, users, struct GameUser, MAX_USERS);
