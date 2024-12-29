#ifndef GAME_USER_H
#define GAME_USER_H

#include "input/input.h"
#include "input/input_common.h"

typedef int game_user_id_t;

#define GAME_USER_DISP_NAME_MAX 32

struct GameUser {
  char display_name[GAME_USER_DISP_NAME_MAX];
  input_device_id_t device_id;
};

struct GameUserOps {
  int (*init_user)(struct GameUser *user, const char *display_name,
                   input_device_id_t device_id);
};

struct GameUserOps *get_game_user_ops(void);

#endif // GAME_USER_H
